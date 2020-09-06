# ロードマップ

1. Intel VT-x を理解する
2. BitVisor のソースコードを読む <- 今ココ
3. BitVisor に改造を加えて動作を確認してみる <- 今ココ
4. ベアメタルデバッガを作る

# 最近何をやったか

- bitvisorの改造をした
- MMIOのフックをした
  - HPETの個数の改ざん(参考： http://qiita.com/deep_tkkn/items/7cbcf22b760790356896 )
  - IOAPICのバージョンの改ざん

# 困っていること

- 今の所なし

# 次は何をやるか

- BitVisor Advent Calendar の解説記事を読む
  - 主にここ( https://qiita.com/mmi/items/18143e7aab1b5679776d )からたどっていく
- PCIデバイスのログを取ってみる
  - PCIバスのフック？
