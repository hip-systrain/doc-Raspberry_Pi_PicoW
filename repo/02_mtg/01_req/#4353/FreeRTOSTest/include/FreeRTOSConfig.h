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
#define configTICK_RATE_HZ                      1000      /* キャストを外し、純粋な数値に修正 */
#define configMAX_PRIORITIES                    32
#define configMINIMAL_STACK_SIZE                ((configSTACK_DEPTH_TYPE)256) /* ワード単位 (256 * 4 = 1024B) */
#define configMAX_TASK_NAME_LEN                 16
//#define configUSE_16_BIT_TICKS                  0 /* 32bitマイコンのため必ず 0 */
#define configTICK_TYPE_WIDTH_IN_BITS           TICK_TYPE_WIDTH_32_BITS /* v11以降・main用 */
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

/* ソフトウェアタイマー設定（xTimerPendFunctionCall を有効にするため必須） */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            256

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