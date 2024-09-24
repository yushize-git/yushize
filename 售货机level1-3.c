#include <stdio.h>
#include <string.h>

#define MAX_CHANNELS 5    // 售货机最大通道数
#define MAX_CAPACITY 50   // 每个通道最多能放50个货物

// 定义通道结构体，存储每个通道的信息
typedef struct {
    char item;  // 货物种类
    int price;  // 货物单价
    int quantity;  // 货物数量
} Channel;

Channel channels[MAX_CHANNELS];  // 定义售货机通道数组

// 初始化所有通道为空
void initializeChannels() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        channels[i].item = '\0';  // 设置货物为空
        channels[i].price = 0;    // 设置价格为0
        channels[i].quantity = 0; // 设置数量为0
    }
}

// 显示当前售货机状态
void displayMachine() {
    for (int i = 0; i < MAX_CHANNELS; i++) {
        if (channels[i].quantity > 0) {
            printf("%d: ", i + 1);
            for (int j = 0; j < channels[i].quantity; j++) {
                printf("%c", channels[i].item);  // 显示货物类型
            }
            printf(" %d\n", channels[i].price);  // 显示货物单价
        }
    }
}

// 在指定通道摆放货物
void placeGoods(char item, int channelNum, int price, int quantity) {
    if (channelNum < 1 || channelNum > MAX_CHANNELS) {
        printf("通道编号无效！\n");
        return;
    }
    
    if (quantity < 1 || quantity > MAX_CAPACITY) {
        printf("货物数量无效！数量必须在1到%d之间。\n", MAX_CAPACITY);
        return;
    }

    channels[channelNum - 1].item = item;
    channels[channelNum - 1].price = price;
    channels[channelNum - 1].quantity = quantity;

    printf("在通道%d中放置了%d个货物%c，单价为%d元\n", channelNum, quantity, item, price);
}

// 从指定通道购买货物
void purchaseGoods(int channelNum, int purchaseQuantity) {
    if (channelNum < 1 || channelNum > MAX_CHANNELS) {
        printf("通道编号无效！\n");
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

    // 投币循环，直到投币金额足够支付总价
    while (totalInserted < totalPrice) {
        printf("请投币（只能投1元、2元或5元）：");
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
}

// 主循环：模拟完整的售货机操作流程
int main() {
    initializeChannels();  // 初始化售货机

    char item;
    int channelNum, price, quantity;
    
    // 摆放货物
    printf("请输入货物种类、通道号、单价和数量：");
    scanf(" %c %d %d %d", &item, &channelNum, &price, &quantity);
    placeGoods(item, channelNum, price, quantity);

    // 显示售货机状态
    displayMachine();

    // 模拟购买货物的过程，直到用户选择退出
    while (1) {
        printf("\n请输入要购买的货物种类、通道号和数量（输入0退出）：");
        scanf(" %c", &item);
        if (item == '0') break;  // 用户输入0表示退出

        scanf("%d %d", &channelNum, &quantity);
        if (channels[channelNum - 1].item != item) {
            printf("错误：通道中没有此类货物！\n");
            continue;
        }

        purchaseGoods(channelNum, quantity);
        
        // 显示售货机状态
        displayMachine();
    }

    printf("售货机操作结束。\n");
    return 0;
}

