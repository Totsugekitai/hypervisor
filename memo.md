# ハイパーバイザの作り方メモ

## 第1回メモ

- vmx root mode と vmx non root mode がある
- vmx non root mode -> vmx root mode : vmexit
- vmx root mode -> vmx non root mode : vmentry
- センシティブ命令で vmx root mode に切り替わる
- どのイベントでvmexitするかはカスタマイズできる（VMCS）
- VMCS にレジスタとか vmexit 要因とかをCPUが記録する

- vt-x を有効化
  - cr4.vmxe = 1
  - vmxon 命令を発行
- VMCS を初期化
  - ページ境界アライメント
  - 専用命令で間接的に設定
  - vmptrld -> vmwrite 命令で設定

- vmptrld で VMCS のアドレスをCPUに伝える

- ハイパーバイザのレジスタを退避
  - vmwrite とか push とか

- ゲストのレジスタを復帰

- vmx non root mode へ vmentry
  - 初めての vmentry 時のみ vmlaunch 命令
  - 二回目以降は vmresume 命令

- ゲストマシンを実行

- ゲストでトラップ要因が発生し、 vmexit

- ゲストのレジスタを退避

- ハイパーバイザのレジスタを復帰

- vmexit 要因を調べ要因に合わせたエミュレーションを行う
  - 次回の vmentry が異なるCPUから実行される可能性がある場合は vmclear 命令を発行して VMCS のアドレスをクリア

- vmptrld で VMCS のアドレスをCPUに伝える

## 第2回メモ

- VMCS の内部構造
  - VMCS-abort indicator
    - vmexit 中のエラー情報が書き込まれる、エラーが起きない限り使われない
  - VMCS data
    - guest-state data / host-state data
      - ゲストホストのデータ。レジスタとか
    - VM-execution control fields
      - ゲスト実行時のCPUの挙動を設定する
      - vmexit のイベントの列挙とかここで行われる
    - VM-exit control fields
      - vmexit 時のCPUの挙動を設定する
      - 外部割り込み要因で vmexit したときの挙動とか
    - VM-entry control fields
      - vmentry 時の挙動を設定
    - VM-exit information fields
      - vmexit 時の要因が書き込まれる

- vmexit 要因リスト
  - 仕様書見て

- vt-x 拡張命令セット
  - ページングとメモリの仮想化
    - シャドーページング
      - ソフトウェア仮想化
      - ゲストのページディレクトリテーブルとページテーブルを複製しておく
    - EPT
      - ハードウェア仮想化
      - VMCS にアドレスを書き込んでおけばいい感じにしてくれる

- VPID
  - TLB の自動フラッシュをいい感じにしてくれる
  - VMCS でオンオフ切り替えられる

## 第3回メモ

- IOデバイス仮想化
  - IOマップドIOの仮想化
    - VMCS を設定してIO割り込みをハンドリングする
    - VM-execution control fields の unconditional IO exiting = 1
      - IOマップドIO全体の割り込みトラップ
    - vmexit reason 30(IO instruction) で vmexit が発生する
    - vmexit information fields の exit qualification で何番ポートが読み書きしたのか
  - メモリマップドIOの仮想化
    - シャドーページングの場合
      - デバイスがマップされたアドレスへのアクセスがあった際にページフォルトを発生させる必要
        - ページテーブルエントリのプレゼントビットを0にする
      - VMCS の VM-execution control fields の exception bitmap を設定
        - ページフォルトで VM exit reason 0 を発生させる
      - VM exit interruption information を参照
      - VM exit qualification を参照し、対象アドレスを特定
      - 読み書きの情報は提供されないので、ripから命令列をデコードし、命令をエミュレーション
    - EPTの場合
      - だいたい同じ
- Local APIC 仮想化
  - 頻繁にアクセスするのでなんか特別な仕組みを作る必要がある
  - APIC access VMExit
    - Local APIC レジスタアクセス専用の vmexit 命令
    - VMCS -> VM-Execution fields -> Virtualize APIC accesses をオン
    - メモリ仮想化がゲスト環境の Local APIC アドレスの範囲に割り当てている物理ページ(APIC access page)のアドレスを VMCS の VM-Execution fields へ設定
    - VM exit qualification からアクセスのあったレジスタとアクセス方向が読める
    - APIC access page に対してページフォルトやEPT Violationを仕掛けているとそっちが優先される

## 第4回メモ
- MSI割り込みはLocal APICに直接送られる

## 第5回メモ
- 内部割り込み -> CPU内で完結するため仮想化の必要なし
  - 仮想CPUへの割り込みの挿入
    - VMCS -> VM-Entry control fields -> VM-entry interruption-information field にベクタ番号と割り込みタイプを書き込む
    - ハイパーバイザがLocal APICのレジスタの値を計算してセットする必要あり
- 外部割り込み -> 仮想化が必要
  - ハイパーバイザでのIO APICとLocal APICの仮想化が必要
  - Local APICの仮想化
    - 
