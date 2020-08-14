#include<stdio.h>
#include <GL/glut.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define    N    52
#define    MIN    0
#define    MAX     52
#define    heart 0
#define    spade  1
#define    diamond 2
#define    club 3
#define    betting 0
#define    start  1
#define    rule 2
#define    end 3
#define    gameover 4
#define    win 0
#define    lose 1
#define    draw 2
#define    brackjack 3
#define    just 4
#define    burst 5
char s[4]={0};
int tip = 5;
int bet = 0;
int mode = betting;
int pmode;
int x,y;
int errar=1;
int result1;
typedef struct cards {
    char num[3];
    int mark;
    int point;
} cards;

cards deck[N];
cards dealer[12];
cards player[12];
int dcount = 0;
int pcount = 0;
int d_judge = 0,p_judge = 0;
void shuffle(void){
    if(mode==gameover){tip=5;}
    dcount=0,pcount=0,d_judge=0,p_judge=0,mode=betting,bet=0,result1=10;
    memset(dealer,0,sizeof(dealer));
    memset(player,0,sizeof(player));
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
        int x = rnd%13+1;
        switch (x) {
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
        mark = rnd/13;
        switch (mark) {
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
void deal_card(void){
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
void output(double x, double y, char *string)
{
  int len, i;

  glRasterPos2f(x, y);
  len = (int) strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
  }
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
int judge_game(void){
    int dsum=0,psum=0;
    int i;
    int result;
    for(i=0;i<dcount;i++){
        dsum += dealer[i].point;
    }
    for(i=0;i<pcount;i++){
        psum += player[i].point;
    }
    if(dsum!=21&&psum==21){
        glColor3d(1.0, 0.0, 0.0);
        if(pcount==2){
            
            result=brackjack;
            tip+=(double)bet*2.5;
        }
        else{
            
            result=just;
            tip+=(double)bet*2.2;
        }
    }
    else if(dsum>21||(dsum<21&&psum>dsum)){
        tip+=bet*2;
        result = win;
    }
    else if(dsum==21||(dsum<21&&psum<dsum)){
        result = lose;
    }
    else{
        tip+=bet;
        result = draw;
    }
    if(tip==0){
        mode = gameover;
    }
    else{
        mode = end;
    }
    glutPostRedisplay();
    return result;
}
void draw_heart(float cx,float cy){
    int i,n=100;
    double pi=3.14159;
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
void draw_diamond(float x,float y){
    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex2d(x-0.04,  y);
    glVertex2d(x,  y-0.07);
    glVertex2d(x+0.04,  y);
    glVertex2d(x,  y+0.07);
    glEnd();
}
void draw_club(double cx,double cy){
    int i,n=100;
    double pi=3.14159;
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
void draw_spade(float cx,float cy){
    int i,n=100;
    double pi=3.14159;
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
void draw_card(float x,float y,cards *card,int count){
    int i;
    float red;
    for(i=0;i<count;i++){
        if(card==dealer&&i==0&&mode==start){
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
        glColor3d(1.0, 1.0, 1.0);
        glBegin(GL_QUADS);
        glVertex2d(x-0.1,  y-0.2);
        glVertex2d(x+0.1,  y-0.2);
        glVertex2d(x+0.1,  y+0.2);
        glVertex2d(x-0.1,  y+0.2);
        glEnd();
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
        glColor3d(red, 0.0, 0.0);
        output(x-0.09,y+0.14,card[i].num);
        output(x+0.05,y-0.18,card[i].num);
        x += 0.21;
    }
}

void disp(void)
 {
     char c1[4];
     char c2[4];
     glClear(GL_COLOR_BUFFER_BIT);
     glColor3d(0.0, 0.0, 0.0);
     output(-0.98,-0.95,"press q for quit");
     if(mode==rule){
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
     else if(mode==betting){
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
         else{
             errar=1;
             tip-=bet;
             mode=start;
             glutPostRedisplay();
         }
     }
     else{
         draw_card(-0.6,0.5,dealer,dcount);
         draw_card(-0.6,-0.5,player,pcount);
         glColor3d(0.0, 0.0, 0.0);
         output(-0.98,-0.85,"press space to show rule");
         output(0.8,0.8,"your tips");
         sprintf(c1, "%d", tip);
         output(0.85,0.7,c1);
         output(0.85,0.6,"bet");
         sprintf(c2, "%d", bet);
         output(0.85,0.5,c2);
         if(mode==start){
               output(-0.15,0.1,"press h for HIT");
               output(0.0,0.0,"or");
               output(-0.15,-0.1,"press s for STAND");
               if(p_judge==2){
                   result1=burst;
                   if(tip<=0){
                       mode=gameover;
                   }
                   else{
                       mode=end;
                   }
               }
               else if(p_judge==1){
                   d_judge = judge_dealer();
                   while(d_judge==0){
                       get_card(dealer,dcount);
                       dcount++;
                       d_judge = judge_dealer();
                       glutPostRedisplay();
                   }
                   result1=judge_game();
               }
             glutPostRedisplay();
         }
         else if(mode==end||mode==gameover){
             if(mode==gameover){
                 glColor3d(1.0, 0.0, 0.0);
                 output(-0.05,-0.15,"GAME OVER");
                 output(-0.06,-0.25,"press r for resset");
             }
             else{
                 output(-0.1,-0.15,"press c for continue");
             }
             switch (result1) {
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
void keyboard(unsigned char key, int x, int y) {
    if(mode==betting){
        static int i=0;
        if(key>='0'&&key<='9'){
            s[i]=key;
            i++;
        }
        else if(key=='\015'){
            bet=atoi(s);
            if(s[0]=='\0'||bet>tip||bet<=0){
                errar=1;
            }
            else{
                errar=0;
            }
            i=0;
            memset( s, '\0', sizeof(s) );
        }
        glutPostRedisplay();
    }
    switch (key) {
        case 'q':
        case 'Q':
        case '\033':  // esc
          exit(0);
          break;
        case 'h':
          if(mode==start){
             get_card(player,pcount);
             pcount++;
             p_judge = judge_player();
             glutPostRedisplay();
          }
          break;
        case 's':
          if(mode==start){
             p_judge=1;
             glutPostRedisplay();
          }
          break;
        case 'r':
          if(mode==gameover){
            init();
            glutPostRedisplay();
          }
          break;
        case ' ':
          pmode=mode;
          mode=rule;
          init();
          glutPostRedisplay();
          break;
        case 'p':
          if(mode==rule){
              mode=pmode;
              init();
              glutPostRedisplay();
          }
         break;
        case 'c':
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
