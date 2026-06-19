# Wii MP3 Player

Wii Homebrewとして動作する、Dolphin Emulator向けのMP3プレイヤー試作です。

devkitPro / libogc を使って、Wiiリモコン操作、仮想SDカードからのMP3読み込み、MP3再生などを実装しています。

## 機能

* Dolphin Emulator上でWii Homebrewとして起動
* `sd:/music` 内の `.mp3` ファイルを読み込み
* 曲リストの表示
* Wiiリモコンによる操作
* MP3の再生 / 停止
* 次の曲 / 前の曲への移動
* 一時停止 / 再開
* SDカード内の曲リスト再読み込み
* 白背景とピンク系文字のシンプルなコンソールUI

## 操作方法

| ボタン            | 操作                 |
| -------------- | ------------------ |
| 十字キー UP / DOWN | 曲を選択               |
| A              | 選択中の曲を再生           |
| B              | 停止                 |
| +              | 次の曲を再生             |
| -              | 前の曲を再生             |
| 1              | `sd:/music` を再読み込み |
| 2              | 一時停止 / 再開          |
| HOME           | 終了                 |

## MP3ファイルの配置

Dolphinの仮想SDカード内に `music` フォルダを作成し、その中にMP3ファイルを入れます。

```text
sd:/music/song1.mp3
sd:/music/song2.mp3
```

Windows環境では、Dolphinの仮想SDカード `WiiSD.raw` を ImDisk などでマウントして編集します。

例：

```text
C:\Users\testa\AppData\Roaming\Dolphin Emulator\Load\WiiSD.raw
```

マウント後、仮想SDカードの直下に `music` フォルダを作成します。

```text
E:\
└─ music
   ├─ song1.mp3
   └─ song2.mp3
```

## 開発環境

* Windows
* devkitPro
* devkitPPC
* libogc
* libfat
* ASND
* MP3Player
* Dolphin Emulator

## ビルド方法

devkitPro MSYS2を起動し、プロジェクトフォルダへ移動して `make` を実行します。

```bash
cd /c/Users/testa/Projects/wii-homebrew-practice
make clean
make
```

ビルドに成功すると `.dol` ファイルが生成されます。
生成された `.dol` ファイルを Dolphin Emulator で起動します。

## 注意点

* 現在のUIは libogc の標準コンソール表示を使っています。
* 日本語ファイル名は文字化けする可能性があります。
* 現時点では英数字の短いファイル名を推奨します。
* MP3ファイルは一度メモリに読み込んでから再生しています。
* 大きすぎるMP3ファイルは再生できない場合があります。
* 現在は試作段階です。

## 今後やりたいこと

* より見やすいGUI
* 日本語フォント表示
* シャッフル再生
* リピート再生
* 曲終了後の自動次曲再生
* 再生時間や進捗バーの表示
* 実機Wiiでの動作確認
