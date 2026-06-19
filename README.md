# Wii MP3 Player

Nintendo Wii向けのシンプルな自作MP3プレイヤーです。

SDカード内のMP3ファイルを読み込み、Wiiリモコンで曲を選択・再生できます。
Dolphin Emulator上では、カスタムForwarder Channelを使ってWiiメニューから起動できます。

## 機能

* `sd:/music` からMP3ファイルを読み込み
* 曲一覧を表示
* Wiiリモコンで曲を選択
* 再生 / 停止 / 一時停止 / 再開
* 次の曲 / 前の曲へ移動
* アプリを再起動せずに音楽フォルダを再読み込み
* DolphinのWiiメニューから起動可能
* カスタムForwarder Channel対応
* カスタムチャンネルアイコン
* カスタムチャンネルバナー
* カスタムチャンネルサウンド

## 操作方法

| ボタン        | 動作           |
| ---------- | ------------ |
| 十字キー 上 / 下 | 曲を選択         |
| A          | 選択中の曲を再生     |
| B          | 停止           |
| +          | 次の曲          |
| -          | 前の曲          |
| 1          | 音楽フォルダを再読み込み |
| 2          | 一時停止 / 再開    |
| HOME       | 終了           |

## SDカード構成

SDカードには以下のようにファイルを配置してください。

```text
sd:/
├─ apps/
│  └─ wii-mp3-player/
│     ├─ boot.dol
│     ├─ meta.xml
│     └─ icon.png
└─ music/
   ├─ song1.mp3
   ├─ song2.mp3
   └─ song3.mp3
```

MP3ファイルは以下のフォルダに入れてください。

```text
sd:/music
```

## Dolphinでのセットアップ

このプロジェクトはDolphin Emulator上で動作確認しています。

DolphinのSDカード同期フォルダは、以下のような構成にしてください。

```text
WiiSDSync/
├─ apps/
│  └─ wii-mp3-player/
│     ├─ boot.dol
│     ├─ meta.xml
│     └─ icon.png
└─ music/
   ├─ song1.mp3
   └─ song2.mp3
```

Dolphin側では以下の設定を行います。

1. SDカードの挿入をエミュレートする
2. SDカードへの書き込みを許可する
3. SDカードのフォルダ同期を有効にする
4. 同期フォルダに `WiiSDSync` フォルダを指定する
5. `boot.dol` を直接起動する、またはForwarder Channel WADをインストールしてWiiメニューから起動する

## Forwarder Channel

このプロジェクトには、Dolphinでの使用を想定したカスタムForwarder Channelが含まれています。

Forwarder Channelは以下のファイルを起動します。

```text
sd:/apps/wii-mp3-player/boot.dol
```

Forwarder Channelには以下の要素を追加しています。

* カスタムアイコン
* カスタムバナー
* カスタムチャンネルサウンド

このWADファイルはDolphinでのテスト用です。

## ビルド方法

このプロジェクトは devkitPro / devkitPPC / libogc を使用しています。

ビルドするには、以下を実行します。

```sh
make
```

生成された `.dol` ファイルを以下の名前に変更してください。

```text
boot.dol
```

その後、以下の場所に配置します。

```text
sd:/apps/wii-mp3-player/boot.dol
```

## 推奨プロジェクト構成

```text
Wii-mp3-player/
├─ source/
│  └─ main.c
├─ build/
├─ assets/
│  ├─ icon.png
│  ├─ banner.png
│  └─ channel_sound.wav
├─ channel/
│  └─ WiiMP3PlayerForwarder_v1.0_Dolphin.wad
├─ sd/
│  ├─ apps/
│  │  └─ wii-mp3-player/
│  │     ├─ boot.dol
│  │     ├─ meta.xml
│  │     └─ icon.png
│  └─ music/
│     └─ put_mp3_files_here.txt
├─ Makefile
└─ README.md
```

## 注意


付属のWADファイルはDolphin Emulatorでのテストを想定しています。
実機WiiにWADファイルをインストールする行為にはリスクがあります。自己責任で使用してください。

## ライセンス

このプロジェクトのソースコードはMIT Licenseで公開しています。

チャンネル素材、WADファイル、使用した外部ツールやライブラリについては、それぞれの作者・ライセンスに従います。
