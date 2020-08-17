#include<stdio.h>
#include <GL/glut.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define    pi   3.14159  //円周率(描画で使用)
#define    N    52  //トランプの枚数
#define    MIN    0  //乱数生成の最小値
#define    MAX     52  //乱数生成の最大値
/*トランプのマーク*/
#define    heart 0
#define    spade  1
#define    diamond 2
#define    club 3
/*ゲームの状態(modeに格納する)*/
#define    betting 0 //賭け金の設定画面
#define    start  1  //ゲームのプレイ画面
#define    rule 2  //ルール説明の画面
#define    end 3  //ゲームの終了画面
#define    gameover 4  //ゲームオーバーの画面(手持ちが0になった時)
/*勝敗*/
#define    win 0  //勝ち
#define    lose 1  //負け
#define    draw 2  //引き分け
#define    brackjack 3  //配られた時点で21
#define    just 4  //3枚以上で21ピッタリ
#define    burst 5  //21を超えた
char s[4]={0};
int tip = 5;  //チップの数(初期状態は5枚)
int bet = 0;  //ベットする数
int mode = betting;  //ゲームの状態(初期状態はbetting)
int pmode;  //前のゲームの状態を保持
int x,y;
int errar=1;  //賭け金が範囲外の時のエラー判定
int result1;  //ゲームの結果
/*トランプの情報*/
typedef struct cards {
    char num[3];  //数字1~K
    int mark;  //マーク(heart,diamond,spade,club)
    int point;  //点数1~11
} cards;
cards deck[N];  //山札
cards dealer[12];  //ディーラーの手札
cards player[12];  //プレイヤーの手札
int dcount = 0;  //ディーラーの手札の数
int pcount = 0;  //プレイヤーの手札の数
int d_judge = 0,p_judge = 0;  //ディーラーの点数判定、プレイヤーの点数判定
/*山札を乱数生成*/
void shuffle(void){
    /*変数の初期化*/
    if(mode==gameover){tip=5;}
    dcount=0,pcount=0,d_judge=0,p_judge=0,mode=betting,bet=0,result1=10;
    memset(dealer,0,sizeof(dealer));
    memset(player,0,sizeof(player));
    int rnd;  //乱数を格納0~51
    int bFind;  //既に生成された乱数を探すフラグ
    int i,j;
    int data[N]={0};  //乱数を格納する配列
    srand((unsigned int)time(NULL));  //seed値を現在時刻に設定
    for(i = 0; i < N; i++){
        do
        {
            bFind = 0;
            rnd = rand() % (MAX - MIN) + MIN;  //乱数生成
            for(j = 0; j < i; j++){//同じ乱数がdataにないか確認
                if(data[j] == rnd){
                    bFind = 1;//見つけたらフラグを立てる
                    break;
                }
            }
        }while(bFind);//同じ乱数がある間繰り返す
        data[i] = rnd;
        int x = rnd%13+1;  //1~13の値がxに格納される。トランプの数字を表す
        memset(deck[i].num, '\0', sizeof(deck[i].num));  //文字列の初期化
        switch (x) {//山札に数字と点数の情報を格納
            case 10:
                deck[i].num[0] = '1';deck[i].num[1] = '0';
                deck[i].point = 10;
                break;
            case 11:
                deck[i].num[0] = 'J';
                deck[i].point = 10;
                break;
            case 12:
                deck[i].num[0] = 'Q';
                deck[i].point = 10;
                break;
            case 13:
                deck[i].num[0] = 'K';
                deck[i].point = 10;
                break;
            case 1:
                deck[i].num[0] = 'A';
                deck[i].point = 11;
                break;
            default:
                deck[i].num[0] = '0' + x;
                deck[i].point = x;
                break;
        }
        int mark = rnd/13;  //0~3の値がmarkに格納される。トランプのマークを表す
        switch (mark) {//山札にマークの情報を格納
            case heart:
                deck[i].mark=heart;
                break;
            case diamond:
                deck[i].mark=diamond;
                break;
            case spade:
                deck[i].mark=spade;
                break;
            case club:
                deck[i].mark=club;
                break;
            default:
                break;
        }
        
    }
}
/*カードを配る関数*/
void deal_card(void){
    dealer[0]=deck[0],dealer[1]=deck[1];
    player[0]=deck[2],player[1]=deck[3];
    dcount+=2;
    pcount+=2;
}
/*山札からカードを引く関数*/
void get_card(cards* deck1,int n){
    static int count = 4;
    deck1[n]=deck[count];
    count++;
}
/*文字を表示する関数*/
void output(double x, double y, char *string)
{
  int len, i;

  glRasterPos2f(x, y);
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
  }
}
/*ディーラーの手札の点数の判定*/
int judge_dealer(void){
    int sum=0,i;
    for(i=0;i<dcount;i++){//手札の合計を出す
        sum += dealer[i].point;
    }
    
    if(sum>21){//バーストしてAが11点で計算されている場合、1で計算し直す
        for(i=0;i<dcount;i++){
            if(dealer[i].point==11){
                dealer[i].point=1;
                sum -= 10;
                break;
            }
        }
        if(sum>16){//それでも合計が17以上の場合1を返す(これ以上引けない)
            return 1;
        }
    }
    else if(sum>16){//バーストしていないが17以上の場合1を返す(これ以上引けない)
        return 1;
    }
    
    return 0;//まだ引ける
}
/*プレイヤーの手札の点数の判定*/
int judge_player(void){
    int sum=0,i;
    for(i=0;i<pcount;i++){//手札の合計を出す
        sum += player[i].point;
    }
    if(sum>21){//バーストしてAが11点で計算されている場合、1で計算し直す
        for(i=0;i<pcount;i++){
            if(player[i].point==11){
                player[i].point=1;
                sum -= 10;
                break;
            }
        }
        if(sum>21){//それでも合計が22以上の場合2を返す(バーストして負け)
            return 2;
        }
    }
    else if(sum==21){//21丁度の場合1を返す(ブラックジャックまたはジャスト)
        return 1;
    }
    return 0;//まだ引ける
}
/*ゲームの結果の判定*/
int judge_game(void){
    int dsum=0,psum=0;//両者の合計を格納
    int result;//結果を格納
    if(dsum!=21&&psum==21){//プレイヤーのみジャストの場合
        if(pcount==2){//ブラックジャック
            
            result=brackjack;
            tip+=(double)bet*2.5;//賭け金の1.5倍がもらえる(賭け金も返されるので2.5倍)
        }
        else{//ジャスト
            
            result=just;
            tip+=(double)bet*2.2;//賭け金の1.2倍がもらえる(賭け金も返されるので2.2倍)
        }
    }
    else if(dsum>21||(dsum<=21&&psum>dsum)){//ディーラーがバーストまたはプレイヤーの方が高得点の場合
        tip+=bet*2;//賭けた分だけもらえる(賭け金も返されるので2倍)
        result = win;//勝ち
    }
    else if(psum<dsum){//ディーラーの方が高得点の場合(プレイヤーはバーストした時点で終了する)
        result = lose;//負け
    }
    else{
        tip+=bet;//賭け金が戻ってくる
        result = draw;//引き分け
    }
    
    if(tip==0){//チップが0の場合ゲームオーバー
        mode = gameover;
    }
    else{
        mode = end;//終了
    }
    glutPostRedisplay();
    return result;
}
/*ハートを描画する関数*/
void draw_heart(float cx,float cy){
    int i,n=100;
    double rate;
    float x,y,r=0.05900/2.0;
    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex2d(cx,  cy-0.09);
    glVertex2d(cx+2.0*r,  cy-0.01);
    for(i=0;i<n;i++){
          rate = (double)i/n;
          x=r*cos(2.0*pi*rate)+cx;
          y=r*sin(2.0*pi*rate)+cy;
          glVertex2d(x+r,y);
    }
    glVertex2d(cx,  cy-r/2);
    for(i=0;i<n;i++){
        rate = (double)i/n;
        x=r*cos(2.0*pi*rate)+cx;
        y=r*sin(2.0*pi*rate)+cy;
        glVertex2d(x-r,y);
    }
    glEnd();
}
/*ダイアを描画する関数*/
void draw_diamond(float x,float y){
    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex2d(x-0.04,  y);
    glVertex2d(x,  y-0.07);
    glVertex2d(x+0.04,  y);
    glVertex2d(x,  y+0.07);
    glEnd();
}
/*クラブを描画する関数*/
void draw_club(double cx,double cy){
    int i,n=100;
    double rate;
    float x,y,r=0.05800/2.0;
    
    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex2d(cx, cy);
    glVertex2d(cx-0.01, cy-0.09);
    glVertex2d(cx+0.01, cy-0.09);
    glVertex2d(cx, cy);
    for(i=-n-5;i<=0;i++){
        rate = (double)i/n;
        x=r*cos(2.0*pi*rate)+cx;
        y=r*sin(2.0*pi*rate)+cy;
        glVertex2d(x+r,y-r);
    }
    glVertex2d(cx, cy);
    for(i=-n-5;i<=0;i++){
        rate = (double)i/n;
        x=r*cos(2.0*pi*rate)+cx;
        y=r*sin(2.0*pi*rate)+cy;
        glVertex2d(x,y+r-0.001);
    }
    for(i=0;i<=n;i++){
        rate = (double)i/n;
        x=r*cos(2.0*pi*rate)+cx;
        y=r*sin(2.0*pi*rate)+cy;
        glVertex2d(x-r,y-r);
    }
    glEnd();
}
/*スペードを描画する関数*/
void draw_spade(float cx,float cy){
    int i,n=100;
    double rate;
    float x,y,r=0.05900/2.0;
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(cx,cy,0.0);
    glRotatef(180.0, 0.0, 0.0, 1.0);
    glTranslatef(-cx,-cy,0.0);
    glColor3d(0.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex2d(cx,  cy-0.09);
    glVertex2d(cx+1.9*r,  cy-0.01);
    for(i=0;i<n;i++){
          rate = (double)i/n;
          x=r*cos(2.0*pi*rate)+cx;
          y=r*sin(2.0*pi*rate)+cy;
          glVertex2d(x+r,y);
    }
    glVertex2d(cx, cy-r/2);
    glVertex2d(cx+0.01, cy+0.07);
    glVertex2d(cx-0.01, cy+0.07);
    glVertex2d(cx, cy-r/2);
    for(i=0;i<n;i++){
        rate = (double)i/n;
        x=r*cos(2.0*pi*rate)+cx;
        y=r*sin(2.0*pi*rate)+cy;
        glVertex2d(x-r,y);
    }
    glEnd();
    glPopMatrix();
}
/*トランプを描画する関数*/
void draw_card(float x,float y,cards *card,int count){
    int i;
    float red;  //マークの色(赤か黒)
    for(i=0;i<count;i++){//手札の枚数分繰り返す
        if(card==dealer&&i==0&&mode==start){//ゲーム中のディーラーの一枚目は伏せておく
            glColor3d(0.6,0.2,0.2);
            glBegin(GL_QUADS);
            glVertex2d(x-0.1,  y-0.2);
            glVertex2d(x+0.1,  y-0.2);
            glVertex2d(x+0.1,  y+0.2);
            glVertex2d(x-0.1,  y+0.2);
            glEnd();
            x += 0.21;
            continue;
        }
        //カードの描画
        glColor3d(1.0, 1.0, 1.0);
        glBegin(GL_QUADS);
        glVertex2d(x-0.1,  y-0.2);
        glVertex2d(x+0.1,  y-0.2);
        glVertex2d(x+0.1,  y+0.2);
        glVertex2d(x-0.1,  y+0.2);
        glEnd();
        //マークの描画
        switch (card[i].mark) {
            case heart:
                draw_heart(x,y);
                red = 1.0;
                break;
            case diamond:
                draw_diamond(x,y);
                red = 1.0;
                break;
            case club:
                draw_club(x,y);
                red = 0.0;
                break;
            case spade:
                draw_spade(x,y);
                red = 0.0;
                break;
            default:
                break;
        }
        //数字の描画
        glColor3d(red, 0.0, 0.0);
        output(x-0.09,y+0.14,card[i].num);
        output(x+0.05,y-0.18,card[i].num);
        x += 0.21;//中心を隣に移す
    }
}
/*画面表示*/
void disp(void)
 {
     char c1[4],c2[4];//数値を描画する際に使用
     glClear(GL_COLOR_BUFFER_BIT);
     glColor3d(0.0, 0.0, 0.0);
     output(-0.98,-0.95,"press q for quit");//終了キーの表示
     if(mode==rule){//ルール説明の表示
         output(-0.98,-0.85,"press p for playing game");
         float x=-0.5,y=0.8;
         glColor3d(0.0, 0.0, 0.0);
         output(x,y,"J,Q,K = 10points  2~10 = 2~10points  A = 1or11points");
         output(x,y-0.2,"-> Dealer and Player start with two cards.");
         output(x,y-0.4,"-> HIT = Draw cards.");
         output(x,y-0.6,"-> but when your hands totales exceed 21, you lose (BURST).");
         output(x,y-0.8,"-> STAND = Compare your total with dealer's total.");
         output(x,y-1.0,"-> The person with the most totale wins!");
         output(x,y-1.2,"-> Dealer draws cards until dealer's total exceed 17.");
     }
     else if(mode==betting){//賭け金設定画面の表示
         sprintf(c1, "%d", tip);
         glColor3d(0.0, 0.0, 0.0);
         output(-0.98,-0.75,"press space to show rule");
         output(-0.1,0.5,"your tips is ");
         output(0.0,0.4,c1);
         output(-0.25,0.3,"How many tips do you bet?");
         output(0.0,0.2,s);
         if(errar==1){
             output(-0.25,0.1,"press number 1~ and enter");
         }
         else{//正しく入力されたらmodeをゲームプレイ状態にして再描画する
             errar=1;
             tip-=bet;
             mode=start;
             glutPostRedisplay();
         }
     }
     else{//ゲーム画面の表示
         //両者の手札を描画
         draw_card(-0.6,0.5,dealer,dcount);
         draw_card(-0.6,-0.5,player,pcount);
         glColor3d(0.0, 0.0, 0.0);
         output(-0.98,-0.85,"press space to show rule");//ルール説明のキーを表示
         //手持ちと賭け金の表示
         output(0.8,0.8,"your tips");
         sprintf(c1, "%d", tip);
         output(0.85,0.7,c1);
         output(0.85,0.6,"bet");
         sprintf(c2, "%d", bet);
         output(0.85,0.5,c2);
         if(mode==start){
             //プレイヤーの選択肢表示
             glColor3d(0.0, 0.0, 0.0);
               output(-0.15,0.1,"press h for HIT");
               output(0.0,0.0,"or");
               output(-0.15,-0.1,"press s for STAND");
               if(p_judge==2){//バーストした場合
                   result1=burst;
                   if(tip<=0){
                       mode=gameover;
                   }
                   else{
                       mode=end;
                   }
               }
               else if(p_judge==1){//STANDした、あるいは21ジャストの場合
                   d_judge = judge_dealer();
                   while(d_judge==0){//ディーラーの点数が17以上になるまで引き続ける
                       get_card(dealer,dcount);
                       dcount++;
                       d_judge = judge_dealer();
                       glutPostRedisplay();
                   }
                   result1=judge_game();//勝敗判定
               }
             glutPostRedisplay();
         }
         else if(mode==end||mode==gameover){
             glColor3d(1.0, 0.0, 0.0);
             if(mode==gameover){//ゲームオーバーの場合
                 
                 output(-0.05,-0.15,"GAME OVER");
                 output(-0.06,-0.25,"press r for resset");//リセットキーの表示
             }
             else{
                 output(-0.1,-0.15,"press c for continue");//続ける場合のキーを表示
             }
             switch (result1) {//結果を表示
                 case win:
                     output(0.0,0.0,"you win!");
                     break;
                 case lose:
                     output(0.0,0.0,"you lose!");
                     break;
                 case draw:
                     output(0.0,0.0,"draw!");
                     break;
                 case brackjack:
                     output(0.0,0.0,"brackjack!");
                     break;
                 case just:
                     output(0.0,0.0,"you win just 21!");
                     break;
                 case burst:
                     output(0.0,0.0,"BURST!");
                     break;
                 default:
                     break;
             }
             glutPostRedisplay();
         }
     }
     glFlush();
     glutSwapBuffers();
 }
/*背景画面*/
 void init(void)
 {
     if(mode==betting||mode==gameover){
      shuffle();
      deal_card();
      p_judge=judge_player();
     }
         float red = 100.0f/255.0f;  // 赤色
         float green = 155.0f/255.0f;    // 緑色
         float blue = 0.0f/255.0f;   // 青色
         float alpha = 0.0f/100.0f;  // 透明度
         glClearColor(red, green, blue, alpha);
 }
/*キーボード入力*/
void keyboard(unsigned char key, int x, int y) {
    if(mode==betting){//賭け金の入力
        static int i=0;
        if(key>='0'&&key<='9'){
            s[i]=key;
            i++;
        }
        else if(key=='\015'/*enter*/){
            bet=atoi(s);
            if(s[0]=='\0'||bet>tip||bet<=0){//入力された賭け金が範囲外の場合エラー
                errar=1;
            }
            else{
                errar=0;
            }
            i=0;
            memset( s, '\0', sizeof(s) );//入力された値の初期化
        }
        glutPostRedisplay();
    }
    switch (key) {
        case 'q':
        case 'Q':
        case '\033':  // esc
          exit(0);//終了
          break;
        case 'h'://HIT:カードを引く
          if(mode==start){
             get_card(player,pcount);
             pcount++;
             p_judge = judge_player();
             glutPostRedisplay();
          }
          break;
        case 's'://STAND:勝負する
          if(mode==start){
             p_judge=1;
             glutPostRedisplay();
          }
          break;
        case 'r'://リセット
          if(mode==gameover){
            init();
            glutPostRedisplay();
          }
          break;
        case ' '://ルール説明
          pmode=mode;//現在のモードを格納
          mode=rule;
          init();
          glutPostRedisplay();
          break;
        case 'p'://ルール説明画面から前の状態に戻る
          if(mode==rule){
              mode=pmode;
              init();
              glutPostRedisplay();
          }
         break;
        case 'c'://勝敗が決まった時ゲームを続ける
          if(mode==end){
            mode=betting;
            init();
            glutPostRedisplay();
          }
          break;
        default:
          break;
        }
  
  
}

int main(int argc, char *argv[])
{
    
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA );
     glutInitWindowPosition( 100, 100 );
     glutInitWindowSize( 1000, 700 );
     glutCreateWindow("Brack_Jack");
     init();
     glutDisplayFunc(disp);
     glutKeyboardFunc(keyboard);
     glutMainLoop();
     return 0;
}
