# FreeRTOS 導入・実装手順書（構成案・骨子）


## 1.導入
### 1.1.FreeRTOS とは（なぜ使うのか？）
* **概要**: マイコン向けの軽量なリアルタイムOS（RTOS）。複数の処理を「タスク」という単位に分割し、CPUの実行時間を自動で割り振る仕組みを提供する。
* **ベアメタル（RTOSなし）との違い**:
  * RTOSなしでは、1つのループの中にすべての処理を詰め込む必要があり、処理が増えるほど時間管理やフラグ管理が複雑化する。
  * FreeRTOS を使うと、「LED制御」「ボタン監視」などを独立した関数として分離でき、`vTaskDelay()` で待機中に他の処理へCPUを譲れるため、見通しが良く応答性の高いプログラムが書ける。
* **本手順書のゴール**: Raspberry Pi Pico（RP2040 / RP2350）に FreeRTOS を組み込み、マルチタスク動作とタスク間通信（キュー）の基本を習得する。


### 1.2.FreeRTOS の入手先
* 公式サイトのzip一括ダウンロードではなく、GitHub上の Raspberry Pi 公式フォークリポジトリ（[raspberrypi/FreeRTOS-Kernel](https://github.com/raspberrypi/FreeRTOS-Kernel)）から取得する（`smp` ブランチを使用）。
* RP2040 / RP2350 のデュアルコア動作や Pico SDK との競合回避があらかじめ対策されているため。


### 1.3.プロジェクトへの取り込み方
* **推奨ディレクトリ構成**: プロジェクトルート（または `lib/`）に `FreeRTOS-Kernel` を配置し、`include/FreeRTOSConfig.h` と `src/` で自作コードと分離する。
* **取得方法**: `git submodule add -b smp` でサブモジュールとして追加する。
* **ビルド設定（`CMakeLists.txt`）**: `PICO_PLATFORM` に応じて RP2040 用（`GCC/RP2040`）と RP2350 用（`GCC/RP2350_ARM_NTZ`）の `.cmake` ファイルを分岐して読み込む。
* **RP2040 / RP2350 の差分**: CPUコア（Cortex-M0+ vs M33）、FPU・TrustZone・MPUの有無、SRAM容量（264KB vs 520KB）の違いと、それに伴う設定差分を記載。


### 1.4.FreeRTOS を取り込んだ際のプロジェクト構成
### 1.3.プロジェクトへの取り込み方（追加後のディレクトリ構成）

既存のプロジェクトルートに `FreeRTOS-Kernel` と設定ファイル用の `include/` ディレクトリを追加します（追加箇所は `★` で表記）。

```text
my_project/
├── .vscode/
│   ├── c_cpp_properties.json   # IntelliSense（コード補完・パス解決）設定
│   ├── extensions.json         # 推奨拡張機能の定義
│   ├── launch.json             # デバッガ（Debug Probe / OpenOCD）構成
│   ├── settings.json           # Pico 拡張機能・CMake・環境変数設定
│   └── tasks.json              # ビルド・フラッシュ書き込みタスク定義
├── build/                      # ビルド成果物（CMake / Ninja の出力先）
│   ├── CMakeCache.txt          # CMake の構成キャッシュ
│   ├── compile_commands.json   # コンパイルデータベース（IntelliSense 連携用）
│   ├── build.ninja             # Ninja ビルドスクリプト
│   └── ...                     # その他の中間ファイル（ビルド後に .uf2 / .elf が生成）
├── FreeRTOS-Kernel/            # ★ [追加] GitHubから取得した公式フォークカーネル本体
├── include/                    # ★ [追加] 自作・設定用ヘッダファイル配置ディレクトリ
│   └── FreeRTOSConfig.h        # ★ [追加] FreeRTOS の各種動作・メモリ・SMP設定ファイル
├── .gitignore                  # Git 管理対象外ファイル（build/ や環境依存設定）の指定
├── CMakeLists.txt              # ★ [編集] FreeRTOS-Kernel と include/ の読み込み設定を追記
├── my_project.cpp              # ★ [編集] メインソースファイル（C の場合は my_project.c）
└── pico_sdk_import.cmake       # Pico SDK のパス解決・読み込み用スクリプト
```


### 1.5.入手先リンク等
* [raspberrypi/FreeRTOS-Kernel (GitHub)](https://github.com/raspberrypi/FreeRTOS-Kernel)（メインで使用）
* [FreeRTOS 公式サイト](https://www.freertos.org/)（APIリファレンス参照用）
* [raspberrypi/pico-examples (GitHub)](https://github.com/raspberrypi/pico-examples)（公式サンプル参照用）


---

## 2.設定
### 2.1.設定ファイル(FreeRTOSConfig.h)の主な項目の意味

| カテゴリ | 主なマクロ名 | 設定の概要・目的 |
| :--- | :--- | :--- |
| **基本動作・スケジューリング** | `configUSE_PREEMPTION`<br>`configTICK_RATE_HZ`<br>`configMAX_PRIORITIES`<br>`configMINIMAL_STACK_SIZE` | プリエンプティブ（横取り型）動作の有効化<br>システムティック周期（1000Hz = 1ms基準）<br>タスク優先度の段階数<br>最小スタックサイズ（ワード単位）の定義|
| **メモリ管理・安全機構** | `configSUPPORT_DYNAMIC_ALLOCATION`<br>`configTOTAL_HEAP_SIZE`<br>`configCHECK_FOR_STACK_OVERFLOW`<br>`configUSE_MALLOC_FAILED_HOOK` | 動的メモリ割り当ての許可<br>FreeRTOS管理ヒープ総容量（RP2040: 128KB / RP2350: 256KB目安）<br>スタック溢れやヒープ枯渇時の異常検知フックの有効化 |
| **Pico / SMP 共通連携** | `configNUMBER_OF_CORES`<br>`configRUN_MULTIPLE_PRIORITIES`<br>`configUSE_CORE_AFFINITY`<br>`configSUPPORT_PICO_*_INTEROP` | 使用コア数（1コア / 2コアSMP）の指定<br>複数優先度タスクの同時実行許可<br>タスクの実行コア固定、Pico SDK の同期・時間関数との競合回避<br>“” |
| **RP2350 (Cortex-M33) 固有** | `configENABLE_FPU`<br>`configENABLE_TRUSTZONE`<br>`configENABLE_MPU`<br>`configMAX_SYSCALL_INTERRUPT_PRIORITY` | ハードウェアFPUの有効化<br>TrustZone（Non-TrustZoneモード動作）<br>MPUの無効化（Non-TrustZoneモード動作）<br>ARMv8-M 向けの割り込み優先度しきい値の定義 |

* **成果物**: 上記設定を網羅し、`PICO_RP2350` マクロで差分を自動切り替えできる `FreeRTOSConfig.h` テンプレートを掲載する。


### 2.2.実際のソースコード
<details>
  <summary>以下に2.1.で示したソースコード（FreeRTOSConfig.h）を記述する</summary>

```C
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* =========================================================================
 * 1. 基本動作・スケジューリング設定
 * ========================================================================= */
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      150000000 /* 実行時にSystemCoreClockで上書きされる */
#define configTICK_RATE_HZ                      ((TickType_t)1000) /* 1ティック = 1ms */
#define configMAX_PRIORITIES                    32
#define configMINIMAL_STACK_SIZE                ((configSTACK_DEPTH_TYPE)256) /* ワード単位 (256 * 4 = 1024B) */
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0 /* 32bitマイコンのため必ず 0 */
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_TASK_NOTIFICATIONS            1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   3

/* =========================================================================
 * 2. 同期・タスク間通信機能（題材6のキュー・ミューテックス用）
 * ========================================================================= */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               8

/* =========================================================================
 * 3. メモリ管理・安全機構設定
 * ========================================================================= */
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1

/* ターゲットマイコンのSRAM容量に応じたヒープサイズ自動切り替え */
#if defined(PICO_RP2350) && (PICO_RP2350 == 1)
    /* RP2350 (SRAM 520KB): 256KB を割り当て */
    #define configTOTAL_HEAP_SIZE               ((size_t)(256 * 1024))
#else
    /* RP2040 (SRAM 264KB): 128KB を割り当て */
    #define configTOTAL_HEAP_SIZE               ((size_t)(128 * 1024))
#endif

#define configAPPLICATION_ALLOCATED_HEAP        0
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP 0

/* 異常検知フック（有効時はソース側でフック関数の定義が必要） */
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_MALLOC_FAILED_HOOK            1
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0

/* =========================================================================
 * 4. Pico / SMP（デュアルコア）共通連携設定
 * ========================================================================= */
#define configNUMBER_OF_CORES                   2 /* 1: シングルコア / 2: デュアルコア(SMP) */
#define configTICK_CORE                         0
#define configRUN_MULTIPLE_PRIORITIES           1
#define configUSE_CORE_AFFINITY                 1
#define configUSE_PASSIVE_IDLE_HOOK             0

/* Pico SDK との競合回避・相互運用設定（raspberrypi/FreeRTOS-Kernel 固有） */
#define configSUPPORT_PICO_SYNC_INTEROP         1
#define configSUPPORT_PICO_TIME_INTEROP         1

/* =========================================================================
 * 5. RP2350 (ARM Cortex-M33) 固有設定
 * ========================================================================= */
#if defined(PICO_RP2350) && (PICO_RP2350 == 1)
    #define configENABLE_FPU                    1 /* ハードウェアFPU有効化 */
    #define configENABLE_TRUSTZONE              0 /* TrustZone無効 (Non-TrustZoneモード) */
    #define configENABLE_MPU                    0 /* MPU無効 */
    #define configRUN_FREERTOS_SECURE_ONLY      1

    /* Cortex-M33 (RP2350) の割り込み優先度設定 (上位4bit有効: 0x00〜0xF0) */
    #define configPRIO_BITS                     4
    #define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0x0f
    #define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
    #define configKERNEL_INTERRUPT_PRIORITY         (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#endif

/* =========================================================================
 * 6. アサート・有効化する標準API関数群
 * ========================================================================= */
#include <assert.h>
#define configASSERT(x)                         assert(x)

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskGetHandle                  1
#define INCLUDE_xTaskResumeFromISR              1
#define INCLUDE_xQueueGetMutexHolder            1

#endif /* FREERTOS_CONFIG_H */
```

</details>


### 2.3.設定に関する補足
### 設定に関する重要な補足メモ

#### フック関数の定義（リンクエラー防止）
`FreeRTOSConfig.h` 内で安全機構として `configCHECK_FOR_STACK_OVERFLOW` と `configUSE_MALLOC_FAILED_HOOK` を有効（`2` / `1`）にしているため、メインのソースファイル（`my_project.cpp` または `.c`）側には、異常発生時に呼び出される以下の2つのフック関数を必ず記述しておく必要があります（未定義のままビルドするとリンクエラーになります）。

```c
#ifdef __cplusplus
extern "C" {
#endif

/* スタックオーバーフロー検知時のフック関数 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    for (;;) {}
}

/* ヒープメモリ確保失敗時のフック関数 */
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for (;;) {}
}

#ifdef __cplusplus
}
#endif
```
※ `.cpp` ファイルからコンパイルする場合でもリンカが正しく関数を見つけられるよう、`extern "C"` で囲んでいます。

#### シングルコア（1コア）への切り替え方法
`3.1` の「1コアで動かす場合」を試す際は、`FreeRTOSConfig.h` 内の以下のマクロ値を `2` から `1` に書き換えるだけで切り替え可能です。

```c
#define configNUMBER_OF_CORES    1  /* 1: シングルコア / 2: デュアルコア(SMP) */
```


---

## 3.2つのコア

### 3.1.1コアで動かす場合と、2コアで動かす場合(SMP)の違い
* **1コア運用（`configNUMBER_OF_CORES 1`）**: Core 0 のみで時分割（並行処理）動作。排他制御がシンプルで、内蔵フラッシュ書き込み時のコアロックアウト等の制約を受けにくい。
* **2コア運用 / SMP（`configNUMBER_OF_CORES 2`）**: 1つのスケジューラが Core 0 と Core 1 を統括し、高優先度タスクを同時並列実行する。スループットが高い反面、スレッド安全性（ミューテックス等の排他制御）への配慮が必要になる。

---

## 4.題材5

### 4.1.複数のタスクを動かす
* **目的**: LED点滅タスクとシリアルログ出力タスクの2つを作成し、異なる周期で独立して動作することを確認する（ベアメタルの単一無限ループとの違いを体感する）。
* **扱う要素**: `xTaskCreate()` によるタスク生成、`vTaskDelay()` によるCPU実行権の譲渡（`sleep_ms()` との違い）、タスク関数の無限ループ構造の注意点。

---

## 5.題材6

### 5.1.タスク間で情報を渡す(キュー)
* **目的**: グローバル変数によるデータ競合を避け、タスク間で安全にデータを送受信する仕組み（FIFOバッファ）を理解する。
* **扱う要素**: `xQueueCreate()`、`xQueueSend()`、`xQueueReceive()` の基本動作と、データ到着までCPUを消費せず待機するブロック動作の仕組み。

---

## 6.完成の目安

### 6.1.LEDタスクとボタンタスクが同時に動き、ボタンを押すとLEDの点滅の速さが変わる。
* **ゴール仕様**: ボタン監視タスク（送信側）でスイッチ押下とチャタリング除去を行い、新しい点滅間隔（ms）をキューへ送信。LED制御タスク（受信側）がキューから値を受け取り、点滅速度をリアルタイムに切り替える。
* **確認ポイント**: 互いのタスクが処理をブロックせず並行・並列動作していること、ボタン押下ごとに点滅周期が切り替わること。


## 付録：用語集

### 1. FreeRTOS 基本・スケジューリング関連
* **タスク（Task）**
  * FreeRTOS における処理の最小単位（独立した関数）。ベアメタルの無限ループを細分化し、役割ごとに並行・並列実行させるために使用する。
* **スケジューラ（Scheduler）**
  * どのタスクにいつCPUの実行権を与えるかを管理・決定する FreeRTOS の中核機能。
* **プリエンプティブ（横取り型）**
  * 優先度の高いタスクが実行可能になった際、現在実行中のタスクを強制的に中断（横取り）して切り替えるスケジューリング方式。
* **アイドルタスク（Idle Task）**
  * 実行可能なタスクが他に存在しない時に自動的に動作する、優先度最低のバックグラウンドタスク。
* **ブロック状態（スリープ待機）**
  * イベント待ちや指定時間（`vTaskDelay` 等）の経過を待つために、CPUを消費せずにタスクが一時停止している状態。

### 2. 時間・メモリ・設定関連
* **システムティック（Tick）**
  * FreeRTOS が時間管理の基準とする一定間隔のタイマー割り込み（本手順書では `1000Hz` = 1ms を基準とする）。
* **ヒープ（Heap）**
  * タスクやキューなどを動的に生成するための FreeRTOS 専用のメモリ領域。
* **スタックサイズ（ワード単位）**
  * 各タスクがローカル変数や関数呼び出しの履歴を保存するために割り当てられるメモリの深さ。Pico などの32bitマイコンでは「1ワード = 4バイト」として計算する。
* **スタックオーバーフロー**
  * タスクに割り当てられたスタック容量を超えてデータが書き込まれ、メモリ領域が破壊される重大な不具合。
* **フック関数（Hook Function）**
  * スタックオーバーフローやヒープ枯渇などの異常が発生した際に、FreeRTOS から自動的に呼び出されるユーザー定義のコールバック関数。

### 3. タスク間通信・同期関連
* **キュー（Queue / FIFOバッファ）**
  * タスク間で安全にデータを送受信するための先入れ先出し（FIFO）構造のバッファ。
* **データ競合**
  * 複数のタスクや割り込みが同じ変数へ同時にアクセスし、値が意図せず破壊される現象。
* **排他制御 / ミューテックス（Mutex）**
  * 共有リソースに同時にアクセスできるタスクを1つに制限し、データ競合を防ぐ仕組み。

### 4. デュアルコア・RP2040 / RP2350 関連
* **SMP（対称型マルチプロセッシング）**
  * 1つの FreeRTOS スケジューラが複数のCPUコア（Core 0 と Core 1）を同時に統括し、タスクを並列実行する仕組み。
* **コアアフィニティ（実行コア固定）**
  * 特定のタスクを特定のCPUコアに「ピン留め」して実行させる機能。
* **ポート / 移植層（Port）**
  * FreeRTOS カーネルを特定のハードウェア（RP2040 や RP2350 など）に適合させるための依存コード群。
* **FPU（浮動小数点演算器）**
  * 浮動小数点（小数）の計算を高速に行うハードウェア回路（RP2350 に搭載）。
* **TrustZone / NTZ（Non-TrustZone）**
  * セキュリティ要件の高い処理と通常処理を分離する ARMv8-M のセキュリティ機能。通常のPico SDK開発では無効（Non-TrustZone）で使用する。
* **MPU（メモリ保護ユニット）**
  * メモリのアクセス権限を制御し、不正な領域への書き込みをハードウェアレベルで検知・防止する機能。