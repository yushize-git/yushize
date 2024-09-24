#include <stdio.h>
#include <string.h>

#define MAX_CHANNELS 5    // 最大通道数
#define MAX_CAPACITY 50   // 每个通道最多放50个货物
#define MAX_UNDO 3        // 最多撤销3步操作

// 定义操作类型
typedef enum {
    PLACE_GOODS,
    PURCHASE_GOODS
} OperationType;

// 记录每次操作的结构体
typedef struct {
    OperationType type;  // 操作类型：摆放货物或购买货物      //结构体定义变量？作用？
    char item;           // 货物种类
    int channelNum;      // 通道编号
    int quantity;        // 数量
    int price;           // 单价（仅对摆放货物有效）
} Operation;

// 定义售货机通道结构体
typedef struct {
    char item;      // 货物种类
    int price;      // 货物单价
    int quantity;   // 货物数量
} Channel;

Channel channels[MAX_CHANNELS];  // 定义售货机通道数组
Operation undoStack[MAX_UNDO];   // 撤销操作的栈
int undoCount = 0;               // 撤销操作计数

// 初始化售货机
void initializeChannels() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        channels[i].item = '\0';  // 设置货物为空
        channels[i].price = 0;    // 设置价格为0
        channels[i].quantity = 0; // 设置数量为0
    }
}

// 显示售货机状态
void displayMachine() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (channels[i].quantity > 0) {
            printf("通道%d: ", i + 1);
            for (int j = 0; j < channels[i].quantity; j++) {
                printf("%c", channels[i].item);
            }
            printf(" 单价：%d元\n", channels[i].price);
        }
    }
}

// 保存操作到撤销栈
void pushUndo(Operation op) {
    if (undoCount < MAX_UNDO) {
        undoStack[undoCount] = op;  // 将操作压入栈中
        undoCount++;
    } else {
        //此时操作已满，将旧操作前移，保留最新三步
        for(int i = 0;i < MAX_UNDO - 1;i++){
            undoStack[i] = undoStack[i + 1];
        }
        undoStack[MAX_UNDO - 1] = op;
    }
}

// 回退操作
void undo() {
    if (undoCount == 0) {
        printf("没有可撤销的操作。\n");
        return;
    }

    undoCount--;  // 获取最近的一次操作
    Operation op = undoStack[undoCount];

    if (op.type == PLACE_GOODS) {
        // 撤销摆放货物：减少相应通道中的货物数量
        channels[op.channelNum - 1].quantity -= op.quantity;
        printf("撤销了通道%d中摆放的%d个货物%c。\n", op.channelNum, op.quantity, op.item);
    } else if (op.type == PURCHASE_GOODS) {
        // 撤销购买操作：增加相应通道中的货物数量
        channels[op.channelNum - 1].quantity += op.quantity;
        printf("撤销了通道%d中购买的%d个货物%c。\n", op.channelNum, op.quantity, op.item);
    }
}

// 在指定通道摆放货物
void placeGoods(char item, int channelNum, int price, int quantity) {
    if (channelNum < 1 || channelNum > MAX_CHANNELS) {
        printf("无效的通道号！\n");
        return;
    }

    if (quantity < 1 || quantity > MAX_CAPACITY) {
        printf("无效的数量！\n");
        return;
    }

    channels[channelNum - 1].item = item;
    channels[channelNum - 1].price = price;
    channels[channelNum - 1].quantity += quantity;

    printf("在通道%d中摆放了%d个货物%c，单价为%d元。\n", channelNum, quantity, item, price);

    // 将摆放操作记录到撤销栈
    Operation op = {PLACE_GOODS, item, channelNum, quantity, price};
    pushUndo(op);
}

// 从指定通道购买货物
void purchaseGoods(int channelNum, int purchaseQuantity) {
    if (channelNum < 1 || channelNum > MAX_CHANNELS) {
        printf("无效的通道号！\n");
        return;
    }

    Channel *ch = &channels[channelNum - 1];

    if (ch->quantity < purchaseQuantity) {
        printf("库存不足！当前库存：%d\n", ch->quantity);
        return;
    }

    int totalPrice = ch->price * purchaseQuantity;
    printf("总价为%d元\n", totalPrice);

    int coin, totalInserted = 0;

    // 投币循环，直到金额足够支付
    while (totalInserted < totalPrice) {
        printf("请投币（1元、2元或5元）：");
        scanf("%d", &coin);

        if (coin == 1 || coin == 2 || coin == 5) {
            totalInserted += coin;
            printf("已投币: %d元，当前总金额: %d元\n", coin, totalInserted);
        } else {
            printf("无效的硬币！\n");
        }
    }

    // 计算找零
    int change = totalInserted - totalPrice;
    if (change > 0) {
        printf("找零: %d元\n", change);
    } else {
        printf("金额刚好，无需找零。\n");
    }

    // 更新库存
    ch->quantity -= purchaseQuantity;

    printf("购买成功！剩余库存：%d\n", ch->quantity);

    // 将购买操作记录到撤销栈
    Operation op = {PURCHASE_GOODS, ch->item, channelNum, purchaseQuantity, 0};
    pushUndo(op);
}

// 主循环，模拟售货机功能
int main() {
    initializeChannels();  // 初始化售货机

    char command[10];
    char item;
    int channelNum, price, quantity;

    while (1) {
        printf("\n请输入操作 (place, buy, undo, exit)：");
        scanf("%s", command);

        if (strcmp(command, "place") == 0) {
            // 摆放货物操作
            printf("请输入货物种类、通道号、单价和数量：");
            scanf(" %c %d %d %d", &item, &channelNum, &price, &quantity);
            placeGoods(item, channelNum, price, quantity);
            displayMachine();
        } else if (strcmp(command, "buy") == 0) {
            // 购买货物操作
            printf("请输入要购买的通道号和数量：");
                        scanf("%d %d", &channelNum, &quantity);
                        if (channels[channelNum - 1].quantity < quantity) {
                            printf("库存不足！\n");
                        } else {
                            purchaseGoods(channelNum, quantity);
                        }
                        displayMachine();
                    } else if (strcmp(command, "undo") == 0) {
                        // 撤销操作
                        undo();
                        displayMachine();
                    } else if (strcmp(command, "exit") == 0) {
                        // 退出程序
                        break;
                    } else {
                        printf("无效命令，请重新输入。\n");
                    }
                }

                printf("售货机操作结束。\n");
                return 0;
            }
