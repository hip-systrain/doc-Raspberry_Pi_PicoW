#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"


// コア0 を動作させるタスク
void test_execute_core0(void *pvParameters)
{
    while (1)
    {
        // 現在のコア番号も確認可能 (portGET_CORE_ID())
        //printf("hello core0! (running on core: %d)\n", portGET_CORE_ID());
        printf("hello core0!\n");
        
        // 他のタスクやアイドルタスクにCPUを譲るため必ずウェイトを入れる
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

// コア1 を動作させるタスク
void test_execute_core1(void *pvParameters)
{
    while (1)
    {
        //printf("hello core1! (running on core: %d)\n", portGET_CORE_ID());
        printf("hello core1!\n");
        
        // 1秒待機
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    printf("stdio_init_all()\n");

    // タスクハンドル格納用変数
    TaskHandle_t taskCore0Handle = NULL;
    TaskHandle_t taskCore1Handle = NULL;

    // コア0用のタスクを作成
    xTaskCreate(
        test_execute_core0,   // タスク関数
        "Core0_Task",         // タスク名（デバッグ用）
        256,                  // スタックサイズ（ワード単位）
        NULL,                 // 引数
        1,                    // 優先度
        &taskCore0Handle      // タスクハンドルを取得
    );

    // コア1用のタスクを作成
    xTaskCreate(
        test_execute_core1,
        "Core1_Task",
        256,
        NULL,
        1,
        &taskCore1Handle
    );

    // 各タスクを特定のコアに固定（アフィニティ設定）
    // ビットマスクで指定: 1 << 0 (0b01) = コア0, 1 << 1 (0b10) = コア1
    if (taskCore0Handle != NULL) {
        vTaskCoreAffinitySet(taskCore0Handle, (1 << 0));
    }
    if (taskCore1Handle != NULL) {
        vTaskCoreAffinitySet(taskCore1Handle, (1 << 1));
    }

    // FreeRTOS スケジューラを開始（タスク実行のためこれ以降の処理には進まない）
    vTaskStartScheduler();

    // ヒープ不足などでスケジューラが起動できなかった場合のみ到達
    while (true) {
        printf("Scheduler failed to start!\n");
        sleep_ms(1000);
    }
}

// スタックオーバーフロー検知時に呼ばれるフック関数
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    while (1) {
        // エラー停止処理
    }
}

// ヒープ（メモリ）確保失敗時に呼ばれるフック関数
void vApplicationMallocFailedHook(void)
{
    while (1) {
        // エラー停止処理
    }
}