11/16


事前準備((Pico Wを動作させるため、Zadigの導入)

1. BOOTSELボタンを押しながらつなげる
2. 「USB Event Notificationのウィンドウが表示されたら」いいえを押す
3. RP2 Boot(Interface 1)が入る
4. WinUSB(v6.1.7600.16385)をインストールする(install Driverを押下)

動作手順(Pico Wの動作確認)

1. VSCode に「Raspberry Pi Pico」拡張機能を入れる
2. 「>Raspberry Pi Pico New Example Pico Project」とVSCodeの上部の検索欄に入力する
(コマンドの実施を意味する)
3. サンプル(LED点滅)からプロジェクトを作る(自分のボードを選ぶ)
(New Example Pico ProjectのUIを表示)
4. ビルドして、BOOTSEL ボタンを押しながらUSBをつなぎ、書き込む
5. オンボードLEDが点滅することを確かめる
