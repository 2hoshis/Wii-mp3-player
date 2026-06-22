# Wii MP3 Player

Nintendo Wii向けのMP3プレイヤーです。

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

このプロジェクトは、Dolphin Emulator上で動作確認しています。

### 1. SDカード同期用フォルダを用意する

まず、DolphinでSDカードとして使うフォルダを用意します。
例として、以下のようなフォルダを作成します。

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

Dolphinから見ると、この `WiiSDSync` フォルダが `sd:/` として扱われます。

そのため、MP3ファイルは以下の場所に配置します。

```text
WiiSDSync/music/
```

Wii側からは以下のパスとして認識されます。

```text
sd:/music
```

---

### 2. DolphinでSDカード設定を有効にする

Dolphinを起動し、以下の設定を行います。

```text
設定
↓
Wii
↓
SDカード
```

以下の項目を有効にしてください。

* SDカードの挿入をエミュレートする
* SDカードへの書き込みを許可する
* SDカードのフォルダ同期を有効にする

その後、同期フォルダとして、先ほど作成した `WiiSDSync` フォルダを指定します。

例：

```text
C:\Users\ユーザー名\Apps\Dolphin\User\Load\WiiSDSync
```

---

### 3. boot.dolを直接起動する場合

Dolphinから直接アプリを起動する場合は、以下のファイルを開きます。

```text
WiiSDSync/apps/wii-mp3-player/boot.dol
```

Dolphinでは以下の操作で起動できます。

```text
ファイル
↓
開く
↓
boot.dol を選択
```

起動後、`sd:/music` にMP3ファイルが入っていれば、曲一覧が表示されます。

---

## Forwarder Channel

このプロジェクトには、Dolphinでの使用を想定したカスタムForwarder Channelがあります。

Forwarder Channelは、Wiiメニュー上に表示されるチャンネルです。
このチャンネルを起動すると、SDカード上の以下のファイルを読み込んで実行します。

```text
sd:/apps/wii-mp3-player/boot.dol
```

つまり、WiiメニューからMP3プレイヤーを起動できるようになります。

### Forwarder Channelの内容

このForwarder Channelには、以下の要素を追加しています。

* カスタムチャンネルアイコン
* カスタムチャンネルバナー
* カスタムチャンネルサウンド

---

### DolphinにForwarder Channel WADをインストールする

DolphinでWADファイルをインストールするには、以下の操作を行います。

```text
ツール
↓
WADをインストール
↓
Forwarder Channel の .wad ファイルを選択
```

インストール後、以下の操作でWiiメニューを起動します。

```text
ツール
↓
Wiiメニューを起動
```

Wiiメニュー上に `Wii MP3 Player` チャンネルが表示されます。

そのチャンネルを起動すると、以下のファイルが実行されます。

```text
sd:/apps/wii-mp3-player/boot.dol
```

---

### 注意

付属のWADファイルは、Dolphin Emulatorでのテストを想定しています。

実機WiiにWADファイルをインストールする行為にはリスクがあります。
実機で使用する場合は自己責任で行ってください。


## ライセンス

このプロジェクトのソースコードはMIT Licenseで公開しています。

チャンネル素材、WADファイル、使用した外部ツールやライブラリについては、それぞれの作者・ライセンスに従います。
