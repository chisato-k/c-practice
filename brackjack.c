#include<stdio.h>
//#include<GL/glut.h>
#include <stdlib.h>
#include <time.h>
// データ数
#define    N    52
// 乱数の最小値
#define    MIN    0
// 乱数の最大値
#define    MAX     52
#define    heart 0
#define    spade  1
#define    diamond 2
#define    club 3
typedef struct cards {
    int num;
    char mark;
    int point;
} cards;
cards deck[N];
cards dealer[12];
cards player[12];
int dcount = 0;
int pcount = 0;

void shuffle(void){
    int rnd;
    int mark;
    int bFind;
    int i,j;
    int data[N]={0};
    srand((unsigned int)time(NULL));
    for(i = 0; i < N; i++){
        do
        {
            bFind = 0;
            rnd = rand() % (MAX - MIN) + MIN;
            for(j = 0; j < i; j++){
                if(data[j] == rnd){
                    bFind = 1;
                    break;
                }
            }
        }while(bFind);
        data[i] = rnd;
        deck[i].num = rnd%13+1;
        mark = rnd/13;
        switch (mark) {
            case heart:
                deck[i].mark='H';
                break;
            case diamond:
                deck[i].mark='D';
                break;
            case spade:
                deck[i].mark='S';
                break;
            case club:
                deck[i].mark='C';
                break;
            default:
                break;
        }
        switch (deck[i].num) {
            case 11:
            case 12:
            case 13:
                deck[i].point = 10;
                break;
            case 1:
                deck[i].point = 11;
                break;
            default:
                deck[i].point = deck[i].num;
                break;
        }
    }
}

void deal_card(void){
    int i;
    dealer[0]=deck[0],dealer[1]=deck[1];
    player[0]=deck[2],player[1]=deck[3];
    dcount+=2;
    pcount+=2;
}

void get_card(cards* deck1,int n){
    static int count = 4;
    deck1[n]=deck[count];
    count++;
}

int judge_dealer(void){
    int sum=0,i;
    for(i=0;i<dcount;i++){
        sum += dealer[i].point;
    }
    
    if(sum>21){
        for(i=0;i<dcount;i++){
            if(dealer[i].point==11){
                dealer[i].point=1;
                sum -= 10;
                break;
            }
        }
        if(sum>16){
            return 1;
        }
    }
    else if(sum>16){
        return 1;
    }
    
    return 0;
}

int judge_player(void){
    int sum=0,i;
    for(i=0;i<pcount;i++){
        sum += player[i].point;
    }
    if(sum>21){
        for(i=0;i<pcount;i++){
            if(player[i].point==11){
                player[i].point=1;
                sum -= 10;
                break;
            }
        }
        if(sum>21){
            return 2;
        }
        else if(sum==21){
            return 1;
        }
        return 0;
    }
    else if(sum==21){
        return 1;
    }
    return 0;
}

int input_screen(){
    int num;
    printf("1.HIT(カードを引く)  2.STAND(現在のカードで勝負)\n");
    printf("Enter the number...");
    scanf("%d",&num);
    return num;
}

void game_screen(int hide){
    int i=0,n=dcount;
    printf("dealer     ");
    if(hide==1){
        n=2;
        i=1;
        printf("|  |");
    }
    for(i;i<n;i++){
        printf("|%c%d|",dealer[i].mark,dealer[i].num);
    }
    printf("\n");
    printf("player     ");
    for(i=0;i<pcount;i++){
        
        printf("|%c%d|",player[i].mark,player[i].num);
    }
    printf("\n");
}
void judge_game(void){
    int dsum=0,psum=0;
    int i;
    for(i=0;i<dcount;i++){
        dsum += dealer[i].point;
    }
    for(i=0;i<pcount;i++){
        psum += player[i].point;
    }
    game_screen(0);
    if(dsum!=21&&psum==21){
        printf("black jack!\n");
    }
    else if(dsum>21||(dsum<21&&psum>dsum)){
        printf("you win!\n");
    }
    else if(dsum==21||(dsum<21&&psum<dsum)){
        printf("you lose!\n");
    }
    else{
        printf("draw!\n");
    }
}
int main()
{
    int d_judge = 0,p_judge = 0;
    int n;
    shuffle();
    deal_card();
    game_screen(1);
    d_judge = judge_dealer();
    p_judge = judge_player();
    
    while(p_judge==0){
        n = input_screen();
        if(n==1){
            get_card(player,pcount);
            pcount++;
            game_screen(1);
            p_judge = judge_player();
        }
        else{
            p_judge=1;
        }
    }
    if(p_judge==2){
        printf("burst!\n");
        return 0;
    }
    while(d_judge==0){
        get_card(dealer,dcount);
        dcount++;
        d_judge = judge_dealer();
    }
    judge_game();
    return 0;
}


/*void display(){
	//pass
}*/
