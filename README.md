# Wio Terminal Time Timer

Wio Terminalを使用した、視覚的に残り時間がわかるタイマーアプリケーションです。
設定した時間が経過すると、画面表示とブザー音でお知らせします。

## 機能

- **視覚的なタイマー表示**: 赤い扇形で残り時間を直感的に表示します。
- **時間設定**: 5ウェイスイッチで時間を1分単位で調整可能（最大60分）。
- **一時停止/再開**: ボタン操作でタイマーの一時停止と再開が可能です。
- **アラーム機能**: 時間が来るとブザーが鳴り、画面に "TIME UP!" と表示されます。

## 必要なもの

- **ハードウェア**
    - [Wio Terminal](https://wiki.seeedstudio.com/Wio-Terminal-Getting-Started/)

- **ソフトウェア**
    - Arduino IDE
    - Wio Terminal ボード定義
    - ライブラリ: `LovyanGFX`

## セットアップ方法

### 1. Arduino IDEの準備

1. Arduino IDEをインストールしていない場合は、[公式サイト](https://www.arduino.cc/en/software)からダウンロードしてインストールしてください。

### 2. Wio Terminalのボード設定

1. Arduino IDEを開き、`File` > `Preferences` (macOS: `Arduino IDE` > `Settings`) を開きます。
2. "Additional Boards Manager URLs" に以下のURLを追加します。
   ```
   https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
   ```
3. `Tools` > `Board` > `Boards Manager...` を開きます。
4. "Wio Terminal" を検索し、`Seeed SAMD Boards` をインストールします。
5. `Tools` > `Board` から `Seeeduino Wio Terminal` を選択します。

### 3. ライブラリのインストール

このプロジェクトでは画面描画に `LovyanGFX` ライブラリを使用しています。

1. `Tools` > `Manage Libraries...` を開きます。
2. "LovyanGFX" を検索し、インストールします。

### 4. 書き込み

1. Wio TerminalをPCにUSB接続します。
2. `Tools` > `Port` でWio Terminalのポートを選択します。
3. `Sketch` > `Upload` (または矢印アイコン) をクリックして書き込みます。

## 使い方

| 操作 | ボタン | 説明 |
| --- | --- | --- |
| **スタート / ストップ** | 上部 左ボタン (WIO_KEY_C) | タイマーのカウントダウンを開始/一時停止します。 |
| **リセット** | 上部 左ボタン (WIO_KEY_C) | タイムアップ後、または一時停止中に長押し気味に押すとリセットされる場合があります（コードロジック依存）。<br>※現在のコードでは、タイムアップ後にボタンを押すとリセットされます。 |
| **時間を増やす** | 5ウェイスイッチ 上 (WIO_5S_UP) | 設定時間を1分増やします（停止中のみ）。 |
| **時間を減らす** | 5ウェイスイッチ 下 (WIO_5S_DOWN) | 設定時間を1分減らします（停止中のみ）。 |

## コードの概要

- `time_timer.ino`: メインのソースコードです。
    - `LovyanGFX` を使用して円グラフを描画しています。
    - `millis()` を使用して非同期で時間をカウントしています。
    - 状態遷移（設定中、動作中、終了）を管理しています。
