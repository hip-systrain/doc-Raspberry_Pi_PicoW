## MicroPythonによる操作の流れ
1. ファームウェアとなる.uf2をインストール
1. picoのbooselボタンを押しながらUSB接続
1. 接続されたpicoのドライブにファームウェアの.uf2をコピペ
1. picoが自動で再起動(ドライブがなくなる)
1. USBは繋げたまま、vscode左メニューのraspberry pi pico projectからnew micropython picoを押下
1. プロジェクト作成画面に遷移
1. Nameにプロジェクト名(任意の名前)を入力
1. Python Versionを選択する
- from python extension
- use system version
- custom path  
の3つの項目が存在。  
おそらく以下の内容だと思われる(要確認)  
  - from python extension：vscodeの拡張でインストールしたPython  
  - use system version：PC側にあるPython  
  - custom path：手動でパスを指定
1. Locationにプロジェクトを保存する場所のパスを記載
1. Createボタンを押下してプロジェクト生成
1. 生成したプロジェクトを開いた状態のvscodeが起動する
1. blink.pyファイルを選択
1. ファイル内にコードを必要に応じて修正(プロジェクト作成時点ではサンプルのコードが記載されている)
1. 画面下のboardconnectedを押下
1. runボタンを押下
1. プログラムが処理されていることを確認する
 
#### 調査中
- シリアルターミナルの接続方法  
  シリアルターミナルに接続すると、vscodeで接続できず、プログラムが起動できない  
  →そもそもの仕様で接続できるのは1つのみなのでvscodeとteratermを同時に接続はできない
- bootselボタンを押しながら接続するとファームウェアとなる.uf2ファイルが消える？

#### 備考
- C/C+と違い、bootselボタンを押しながら接続してrunするという手順はない(接続中であればそのままrunの実行が出来る)
- STOPボタンで終了できる
- RUN中でなけれは変更したコードを次にrunをしたときに自動で反映する
- 文字列の表示はvscodeのターミナルで確認できる