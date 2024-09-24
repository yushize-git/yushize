#include <stdio.h>
#include <string.h>

#define MAX_CHANNELS 5    // 最大通道数
#define MAX_CAPACITY 50   // 每个通道最多放 50 个货物
#define MAX_UNDO 3        // 最多保留最近三步操作

int totalCost = 0;

// 定义操作类型
typedef enum {
    PLACE_GOODS,
    PURCHASE_GOODS
} OperationType;

// 记录每次操作的结构体
typedef struct {
    OperationType type;  // 操作类型：摆放货物或购买货物
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
        channels[i].price = 0;    // 设置价格为 0
        channels[i].quantity = 0; // 设置数量为 0
    }
}

// 显示售货机状态
void displayMachine() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (channels[i].quantity > 0) {
            printf("通道%d: %c, 数量: %d, 单价: %d 元\n", i + 1, channels[i].item, channels[i].quantity, channels[i].price);
        }
    }
}

// 保存操作到撤销栈，并维护只保留最近三步
void pushUndo(Operation op) {
    if (undoCount < MAX_UNDO) {
        undoStack[undoCount] = op;
        undoCount++;
    } else {
        // 若已满，将旧的操作移除，只保留最近三步
        for (int i = 0; i < MAX_UNDO - 1; i++) {
            undoStack[i] = undoStack[i + 1];
        }
        undoStack[MAX_UNDO - 1] = op;
    }
}

// 回退操作，只撤销最近三步
void undo() {
    if (undoCount == 0) {
        printf("没有可撤销的操作。\n");
        printf("已达到最大撤销次数，无法撤销。\n");
        return;
    }
    
    undoCount--;
    Operation op = undoStack[undoCount];
    
    if (op.type == PLACE_GOODS) {
        // 撤销摆放货物：减少相应通道中的货物数量
        channels[op.channelNum - 1].quantity -= op.quantity;
        printf("撤销了通道%d中摆放的%d个货物%c。\n", op.channelNum, op.quantity, op.item);
    } else if (op.type == PURCHASE_GOODS) {
        // 撤销购买操作：增加相应通道中的货物数量
        channels[op.channelNum - 1].quantity += op.quantity;
        printf("撤销了通道%d中购买的%d个货物%c。\n", op.channelNum, op.quantity, op.item);
        totalCost -= channels[op.channelNum - 1].price;
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
    
    printf("在通道%d中摆放了%d个货物%c，单价为%d 元。\n", channelNum, quantity, item, price);
    
    // 将摆放操作记录到撤销栈
    Operation op = {PLACE_GOODS, item, channelNum, quantity, price};
    pushUndo(op);
}

// 处理货物购买
int purchaseGoods(char item, int channelNum, int purchaseQuantity) {
    if (channelNum < 1 || channelNum > MAX_CHANNELS) {
        printf("无效的通道号！\n");
        return -1;
    }
    
    Channel *ch = &channels[channelNum - 1];
    
    if (ch->quantity < purchaseQuantity) {
        printf("库存不足！当前库存：%d\n", ch->quantity);
        return -1;
    }
    
    // 判断购买的商品在该通道中是否存在
    if (ch->item != item) {
        printf("该通道中不存在要购买的商品。\n");
        return -1;
    }
    
    int totalPrice = ch->price * purchaseQuantity;
    ch->quantity -= purchaseQuantity;  // 更新库存
    printf("购买成功！从通道%d中购买了%d个货物%c，目前总价为%d 元。\n", channelNum, purchaseQuantity, ch->item, totalPrice);
    
    // 将购买操作记录到撤销栈
    Operation op = {PURCHASE_GOODS, ch->item, channelNum, purchaseQuantity, 0};
    pushUndo(op);
    
    return totalPrice;
}

// 主循环，模拟售货机功能
int menu(char command[10], char item, int channelNum, int price, int quantity, int totalInserted) {
    
    totalCost = 0;
    
    //显示当前售卖机状态
    displayMachine();
    
    // 货物购买
    printf("\n开始选择购买的货物（输入 END 结束）：\n");
    while (1) {
        scanf("%s", command);
        if (strcmp(command, "END") == 0) {
            break;
        } else if (strcmp(command, "back") == 0) {
            undo();  // 撤销操作
            continue;
        }
        item = command[0];  // 读取货物种类
        scanf("%d %d", &channelNum, &quantity);
        int cost = purchaseGoods(item,channelNum, quantity);
        if (cost >= 0) {
            totalCost += cost;  // 累计购买总价
        }
    }
    
    // 投币
    if (totalCost == 0){
        printf("无需付款!\n");
    }else{
        printf("\n总价为%d 元，请开始投币：\n", totalCost);
        while (totalInserted < totalCost) {
            int coin;
            scanf("%d", &coin);
            if (coin == 1 || coin == 2 || coin == 5) {
                totalInserted += coin;
                printf("已投币: %d 元，当前总金额: %d 元\n", coin, totalInserted);
            } else {
                printf("无效的硬币！\n");
            }
        }
        
        // 计算找零
        int change = totalInserted - totalCost;
        if (change > 0) {
            printf("找零: %d 元\n", change);
        } else {
            printf("金额刚好，无需找零。\n");
        }
    }
    printf("继续交易？(输入任意字母继续，输入END结束)\n");
    scanf("%s", command);
    if (strcmp(command, "END") == 0) {
        return -1;
    } else if (strcmp(command, "back") == 0) {
        undo();  // 撤销操作
    } else {
        menu(command, item, channelNum, price, quantity, totalInserted);
    }
    
    return 0;
}

int main() {
    char command[10];
    char item;
    int channelNum, price, quantity, totalInserted = 0;
    
    initializeChannels();  // 初始化售货机
    
    // 货物摆放
    printf("开始摆放货物（输入 END 结束）：\n");
    while (1) {
        scanf("%s", command);
        if (strcmp(command, "END") == 0) {
            break;
        } else if (strcmp(command, "back") == 0) {
            undo();  // 撤销操作
            continue;
        }
        item = command[0];  // 读取货物种类
        scanf("%d %d %d", &channelNum, &price, &quantity);
        placeGoods(item, channelNum, price, quantity);
    }
    menu(command, item, channelNum, price, quantity, totalInserted);
}

