#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
//#include <gtk/gtk.h>
#include <unistd.h> //nanosleep(&ps, NULL); struct timespec ps;

int ArBase[4194304]={0};
int ArPant[16793604]={0};
int BaseF=1;
int BaseC=1;
int Entry=0;
int Exit=0;

int ini_i,ini_j,ext_i,ext_j;
int ready=0;

struct subject{//guarda el avanze sobre el laberinto
    int i;//posicion
    int j;
    int dir;//8 up, 4 left , 2 down, 1 right, 0 stay
    int sum_ang;
    int fordw;  //via avanzar
    int rev;    //via retroceder
    int r;  //colores
    int g;
    int b;
}raton,mano_izq,mano_der,pledge,tremaux;

int SpwTrR[2796203]={0};
int SpwTrC[2796203]={0};
int stp;

char* archivo;

int guardar();
int abrir();
void Pasar();

int resolv(int m,int n,int ar[][n]);
void addFrontier(int m, int n,int i, int j,int ar[][n]);
void removeBarrier(int m, int n,int i, int j,int ar[][n]);
void setentrys(int m, int n,int ar[][n]); //n m > 1
void generate_maze();
void desplegar ();
int abrir_maze(char* archivo);
void guardar_maze(char* archivo);

void setij();
int random_mouse(int m,int n, int ar[][n]);
void run();


int main(int argc, char *argv[])
{
    srand(time(NULL));
    /*BaseF=20;
    BaseC=20;
    resolv(BaseF,BaseC,&ArBase);
    generate_maze();
    guardar_maze("pr1.txt");*/

    abrir_maze("pr1.txt");
    //desplegar();
    setij();
    run();
    desplegar();
    return 0;
}

int resolv(int m,int n,int ar[][n]){
    int spawn = rand() % (n*m);
    int mov;
    int i,j;
    stp=0;
    Entry=Exit=0;
    for(i=0;i<m;i++){
        for(j=0;j<n;j++)
            ar[i][j]=0;
    }
    i=spawn/n;
    j=spawn%n;
    ar[i][j]=16;
    addFrontier(m,n,i,j,ar);
    while(stp>0){
        mov=0;
        if(stp>0)
            mov = rand()%stp;
        i=SpwTrR[mov];
        j=SpwTrC[mov];
        SpwTrR[mov]=SpwTrR[stp-1];
        SpwTrC[mov]=SpwTrC[stp-1];
        stp--;
        removeBarrier(m,n,i,j,ar);
        addFrontier(m,n,i,j,ar);
    }
    i=spawn/n;
    j=spawn%n;
    ar[i][j]-=16;
    setentrys(m,n,ar);
    return 1;
}

void addFrontier(int m, int n,int i, int j,int ar[][n]){
    if (0<=i-1){
        if(ar[i-1][j]==0){
            SpwTrR[stp]=i-1;
            SpwTrC[stp]=j;
            ar[i-1][j]=-1;
            stp++;
        }
    }
    if (0<=j-1){
        if(ar[i][j-1]==0){
            SpwTrR[stp]=i;
            SpwTrC[stp]=j-1;
            ar[i][j-1]=-1;
            stp++;
        }
    }
    if (i+1<m){
        if(ar[i+1][j]==0){
            SpwTrR[stp]=i+1;
            SpwTrC[stp]=j;
            ar[i+1][j]=-1;
            stp++;
        }
    }
    if (j+1<n){
        if(ar[i][j+1]==0){
            SpwTrR[stp]=i;
            SpwTrC[stp]=j+1;
            ar[i][j+1]=-1;
            stp++;
        }
    }
}

void removeBarrier(int m, int n,int i, int j,int ar[][n]){
    int select=0;
    int pos[4]={0};
    int resp,temp;
    if (0<=i-1){
        if(ar[i-1][j]>0){
            pos[select]=4;
            select++;
        }
    }
    if (0<=j-1){
        if(ar[i][j-1]>0){
            pos[select]=3;
            select++;
        }
    }
    if (i+1<m){
        if(ar[i+1][j]>0){
            pos[select]=2;
            select++;
        }
    }
    if (j+1<n){
        if(ar[i][j+1]>0){
            pos[select]=1;
            select++;
        }
    }
    temp=rand()%select;
    resp=pos[temp];
    if (resp==1){
        ar[i][j+1]+=4;
        ar[i][j]=1;
    }
    else if (resp==2){
        ar[i+1][j]+=8;
        ar[i][j]=2;
    }
    else if (resp==3){
        ar[i][j-1]+=1;
        ar[i][j]=4;
    }
    else if (resp==4){
        ar[i-1][j]+=2;
        ar[i][j]=8;
    }
}

void setentrys(int m, int n,int ar[][n]){
    int ei,ej,si,sj,ig,p;
    ig=1;
    ei=ej=si=sj=0;
    if(m>1){
        ei=rand()% m;
        si=rand()% m;
    }
    if(n>1){
        ej=rand()% n;
        sj=rand()% n;
    }
    int pos[4]={0,ei,m-1,si};
    int alt[4]={ej,n-1,sj,0};
    p=rand()%4;
    ei=pos[p];
    ej=alt[p];
    while (ig){
        p=rand()%4;
        si=pos[p];
        sj=alt[p];
        if(ei*n+ej!=si*n+sj)
            ig=0;
    }
    Entry=ei*n+ej;
    Exit=si*n+sj;
    if (ej==n-1){
        ar[ei][ej]+=1;
    }
    else if (ei==m-1){
        ar[ei][ej]+=2;
    }
    else if (ej==0){
        ar[ei][ej]+=4;
    }
    else if (ei==0){
        ar[ei][ej]+=8;
    }

    if (sj==n-1){
        ar[si][sj]+=1;
    }
    else if (si==m-1){
        ar[si][sj]+=2;
    }
    else if (sj==0){
        ar[si][sj]+=4;
    }
    else if (si==0){
        ar[si][sj]+=8;
    }
}

void generate_maze(){
    int t,i,j,n,act,E,S,ex;
    n=act=0;
    for(i =0; i<BaseF*2;i++){
        for(j =0; j<BaseC;j++){
            t=i>>1;
            ex=(BaseC*t+j);
            act=ArBase[ex];
            if(!(i&1)){
                n++;
                if (act&8){
                    ArPant[n]=1;
                    if (ex==Exit)
                        S=n;
                    else if(ex==Entry)
                        E=n;
                }
            }else{
                if (act&4){
                    ArPant[n]=1;
                    if (ex==Exit)
                        S=n;
                    else if(ex==Entry)
                        E=n;
                }
                n++;
                ArPant[n]=1;
            }
            n++;
        }
        if((act&1)&&(i&1)){
            ArPant[n]=1;
            if (ex==Exit)
                S=n;
            else if(ex==Entry)
                E=n;
        }
        n++;
    }
    for(j=0;j<BaseC;j++){
        n++;
        ex=(BaseF-1)*BaseC+j;
        act=ArBase[ex];
        if (act&2){
            ArPant[n]=1;
            if (ex==Exit)
                S=n;
            else if(ex==Entry)
                E=n;
        }
        n++;
    }
    Exit=S;
    Entry=E;
}

void desplegar(){
    int l=(2*BaseF+1)*(2*BaseC+1);
    int n=(2*BaseC+1);
    //int tam=10;
    for(int i = 0;i<l;i++){
        if(i%n==0)
            printf("\n");
        if(ArPant[i]==0)
            printf("XX");
        else if(ArPant[i]==1)
            printf("  ");
        else
            printf("**");
    }
}

void guardar_maze(char* archivo){
    FILE *fp;
    fp=fopen(archivo, "wb");
    fwrite((const void*)&BaseF, sizeof(int), 1, fp);
    fwrite((const void*)&BaseC, sizeof(int), 1, fp);
    fwrite((const void*)&Entry, sizeof(int), 1, fp);
    fwrite((const void*)&Exit, sizeof(int), 1, fp);
    fwrite(ArPant, sizeof(int), (2*BaseF+1)*(2*BaseC+1), fp);
    fclose(fp);
}

int abrir_maze(char* archivo){
    FILE *fp;
    if( access( archivo, F_OK ) != -1 ) {
        fp=fopen(archivo, "rb");
        fread(&BaseF, sizeof(int), 1, fp);
        fread(&BaseC, sizeof(int), 1, fp);
        fread(&Entry, sizeof(int), 1, fp);
        fread(&Exit, sizeof(int), 1, fp);
        fread(ArPant, sizeof(int), (2*BaseF+1)*(2*BaseC+1), fp);
        fclose(fp);
        return 1;
    } else
        return 0;
}

/*__________________________________________*/

void setij(){
    ini_i=Entry/(2*BaseC+1);
    ini_j=Entry%(2*BaseC+1);
    ext_i=Exit/(2*BaseC+1);
    ext_j=Exit%(2*BaseC+1);
    raton.i=mano_izq.i=mano_der.i=pledge.i=ini_i;
    raton.j=mano_izq.j=mano_der.j=pledge.j=ini_j;
    int dir=0;
    if(ini_i==0){
        dir=2;
        //ini_i+=1;
    }
    else if(ini_i==2*BaseF){
        dir=8;
        //ini_i-=1;
    }
    else if(ini_j==0){
        dir=1;
        //ini_j+=1;
    }
    else{
        dir=4;
        //ini_j-=1;
    }
    raton.dir=mano_izq.dir=mano_der.dir=pledge.dir=tremaux.dir=dir;
    raton.sum_ang=mano_izq.sum_ang=mano_der.sum_ang=pledge.sum_ang=tremaux.sum_ang=0;
    raton.fordw=2;
    raton.rev=4096;
    mano_izq.fordw=4;
    mano_izq.rev=2048;
    mano_der.fordw=8;
    mano_der.rev=1024;
    pledge.fordw=16;
    pledge.rev=512;
    tremaux.fordw=32;
    tremaux.rev=256;
    raton.i=mano_izq.i=mano_der.i=pledge.i=tremaux.i=ini_i;
    raton.j=mano_izq.j=mano_der.j=pledge.j=tremaux.j=ini_j;
}

int dir_avanc(struct subject *s,int m,int n, int ar[][n]){
    int resp=0;
    if(ar[s->i][s->j+1]!=0&&((s->i!=ini_i)||(s->j+1!=ini_j)))
        resp=1;
    if(ar[s->i+1][s->j]!=0&&((s->i+1!=ini_i)||(s->j!=ini_j)))
        resp+=2;
    if(ar[s->i][s->j-1]!=0&&((s->i!=ini_i)||(s->j-1!=ini_j)))
        resp+=4;
    if(ar[s->i-1][s->j]!=0&&((s->i-1!=ini_i)||(s->j!=ini_j)))
        resp+=8;
    if ((s->dir&8)&&(resp&2))
        resp-=2;
    else if ((s->dir&2)&&(resp&8))
        resp-=8;
    else if ((s->dir&1)&&(resp&4))
        resp-=4;
    else if ((s->dir&4)&&(resp&1))
        resp-=1;
    return resp;
}

int dir_avanc_trem(struct subject *s,int m,int n, int ar[][n]){
    int resp=0;
    if(ar[s->i][s->j+1]!=0&&!(s->rev&ar[s->i][s->j+1]&&s->fordw&ar[s->i][s->j+1])&&((s->i!=ini_i)||(s->j+1!=ini_j)))
        resp=1;
    if(ar[s->i+1][s->j]!=0&&!(s->rev&ar[s->i+1][s->j]&&s->fordw&ar[s->i+1][s->j])&&((s->i+1!=ini_i)||(s->j!=ini_j)))
        resp+=2;
    if(ar[s->i][s->j-1]!=0&&!(s->rev&ar[s->i][s->j-1]&&s->fordw&ar[s->i][s->j-1])&&((s->i!=ini_i)||(s->j-1!=ini_j)))
        resp+=4;
    if(ar[s->i-1][s->j]!=0&&!(s->rev&ar[s->i-1][s->j]&&s->fordw&ar[s->i-1][s->j])&&((s->i-1!=ini_i)||(s->j!=ini_j)))
        resp+=8;
    if ((s->dir&8)&&(resp&2))
        resp-=2;
    else if ((s->dir&2)&&(resp&8))
        resp-=8;
    else if ((s->dir&1)&&(resp&4))
        resp-=4;
    else if ((s->dir&4)&&(resp&1))
        resp-=1;
    return resp;
}

int simple_path(int avn){//solo un camino posible = n,camino cerrado y multiples caminos=0
    int resp=0;
    if (avn&1)
        resp+=1;
    if (avn&2)
        resp+=1;
    if (avn&4)
        resp+=1;
    if (avn&8)
        resp+=1;
    if (resp==1)
        return avn;
    else
        return 0;
}

int go_right(struct subject *s,int dir,int m,int n, int ar[][n]){
    if(s->dir!=dir){
        if(s->dir==8&&(dir&1)){
            s->j+=1;
            dir=1;
        }else if(s->dir==4&&(dir&8)){
            s->i-=1;
            dir=8;
        }else if(s->dir==2&&(dir&4)){
            s->j-=1;
            dir=4;
        }else if(s->dir==1&&(dir&2)){
            s->i+=1;
            dir=2;
        }else
            return 0;
        s->dir=dir;
        s->sum_ang+=90;
        if(s->dir&8||s->dir&4)
            ar[s->i][s->j]+=s->rev;
        else
            ar[s->i][s->j]+=s->fordw;
        return 1;
    }
    return 0;
}
int go_left(struct subject *s,int dir,int m,int n, int ar[][n]){
    if(s->dir!=dir){
        if(s->dir==8&&(dir&4)){
            s->j-=1;
            dir=4;
        }else if(s->dir==4&&(dir&2)){
            s->i+=1;
            dir=2;
        }else if(s->dir==2&&(dir&1)){
            s->j+=1;
            dir=1;
        }else if(s->dir==1&&(dir&8)){
            s->i-=1;
            dir=8;
        }else
            return 0;
        s->dir=dir;
        s->sum_ang-=90;
        if(s->dir&8||s->dir&4)
            ar[s->i][s->j]+=s->rev;
        else
            ar[s->i][s->j]+=s->fordw;
        return 1;
    }
    return 0;
}
int go_straight(struct subject *s,int dir,int m,int n, int ar[][n]){
    if(s->dir&dir){
        if(s->dir==8){
            s->i-=1;
        }else if(s->dir==4){
            s->j-=1;
        }else if(s->dir==2){
            s->i+=1;
        }else if(s->dir==1){
            s->j+=1;
        }
        if(s->dir&8||s->dir&4)
            ar[s->i][s->j]+=s->rev;
        else
            ar[s->i][s->j]+=s->fordw;
        return 1;
    }
    return 0;
}
void go_back(struct subject *s,int m,int n, int ar[][n]){
    if(s->dir==8){
        s->dir=2;
        s->i+=1;
    }else if(s->dir==4){
        s->dir=1;
        s->j+=1;
    }else if(s->dir==2){
        s->dir=8;
        s->i-=1;
    }else if(s->dir==1){
        s->dir=4;
        s->j-=1;
    }
    if(s->sum_ang<0)
        s->sum_ang-=180;
    else
        s->sum_ang+=180;
    if(s->dir&8||s->dir&4)
        ar[s->i][s->j]+=s->rev;
    else
        ar[s->i][s->j]+=s->fordw;
}

int random_mouse(int m,int n, int ar[][n]){
    if(raton.i==ext_i&&raton.j==ext_j)
        return 1;
    else{
        int dir_avan=dir_avanc(&raton,m,n,ar);
        if(dir_avan==0){
            go_back(&raton,m,n,ar);
            return 0;
        }
        else if(simple_path(dir_avan)){
            if(go_straight(&raton,dir_avan,m,n,ar))
                return 0;
            else if(go_left(&raton,dir_avan,m,n,ar))
                return 0;
            else if(go_right(&raton,dir_avan,m,n,ar))
                return 0;
        }
        else{
            int selec[4]={(dir_avan&1),(dir_avan&2),(dir_avan&4),(dir_avan&8)};
            int random=rand()%4;
            while(selec[random]==0)//quitar while si raton puede devolverse
                random=rand()%4;
            dir_avan=selec[random];
            if(go_straight(&raton,dir_avan,m,n,ar))
                return 0;
            else if(go_left(&raton,dir_avan,m,n,ar))
                return 0;
            else if(go_right(&raton,dir_avan,m,n,ar))
                return 0;
            else
                go_back(&raton,m,n,ar);
            return 0;
        }
    }
}


int left_hand(int m,int n, int ar[][n]){
    if(mano_izq.i==ext_i&&mano_izq.j==ext_j)
        return 1;
    else{
        int dir_avan=dir_avanc(&mano_izq,m,n,ar);
        if(dir_avan==0){
            go_back(&mano_izq,m,n,ar);
            return 0;
        }
        else if(simple_path(dir_avan)){
            if(go_straight(&mano_izq,dir_avan,m,n,ar))
                return 0;
            else if(go_left(&mano_izq,dir_avan,m,n,ar))
                return 0;
            else if(go_right(&mano_izq,dir_avan,m,n,ar))
                return 0;
        }
        else{
            if(go_left(&mano_izq,dir_avan,m,n,ar))
                return 0;
            else if(go_straight(&mano_izq,dir_avan,m,n,ar))
                return 0;
            else if(go_right(&mano_izq,dir_avan,m,n,ar))
                return 0;
        }
    }
}

int right_hand(int m,int n, int ar[][n]){
    if(mano_der.i==ext_i&&mano_der.j==ext_j)
        return 1;
    else{
        int dir_avan=dir_avanc(&mano_der,m,n,ar);
        if(dir_avan==0){
            go_back(&mano_der,m,n,ar);
            return 0;
        }
        else if(simple_path(dir_avan)){
            if(go_straight(&mano_der,dir_avan,m,n,ar))
                return 0;
            else if(go_left(&mano_der,dir_avan,m,n,ar))
                return 0;
            else if(go_right(&mano_der,dir_avan,m,n,ar))
                return 0;
        }
        else{
            if(go_right(&mano_der,dir_avan,m,n,ar))
                return 0;
            else if(go_straight(&mano_der,dir_avan,m,n,ar))
                return 0;
            else if(go_left(&mano_der,dir_avan,m,n,ar))
                return 0;
        }
    }
}

int pledge_alg(int m,int n, int ar[][n]){
    if(pledge.i==ext_i&&pledge.j==ext_j)
        return 1;
    else{
        int dir_avan=dir_avanc(&pledge,m,n,ar);
        if(dir_avan==0){
            go_back(&pledge,m,n,ar);
            return 0;
        }
        else if(simple_path(dir_avan)){
            if(go_straight(&pledge,dir_avan,m,n,ar))
                return 0;
            else if(go_left(&pledge,dir_avan,m,n,ar))
                return 0;
            else if(go_right(&pledge,dir_avan,m,n,ar))
                return 0;
        }
        else{
            if(pledge.sum_ang==0){
                int selec[4]={(dir_avan&1),(dir_avan&2),(dir_avan&4),(dir_avan&8)};
                int random=rand()%4;
                while(selec[random]==0)
                    random=rand()%4;
                dir_avan=selec[random];
            }
            if(pledge.sum_ang<0){
                if(go_left(&pledge,dir_avan,m,n,ar))
                    return 0;
                else if(go_straight(&pledge,dir_avan,m,n,ar))
                    return 0;
                else if(go_right(&pledge,dir_avan,m,n,ar))
                    return 0;
                else
                    go_back(&pledge,m,n,ar);
                return 0;
            }
            else{
                    if(go_right(&pledge,dir_avan,m,n,ar))
                    return 0;
                else if(go_straight(&pledge,dir_avan,m,n,ar))
                    return 0;
                    else if(go_left(&pledge,dir_avan,m,n,ar))
                    return 0;
                else
                    go_back(&pledge,m,n,ar);
                return 0;
            }
        }
    }
}

int tremaux_alg(int m,int n, int ar[][n]){
    if(tremaux.i==ext_i&&tremaux.j==ext_j)
        return 1;
    else{
        int dir_avan=dir_avanc_trem(&tremaux,m,n,ar);
        if(dir_avan==0){
            go_back(&tremaux,m,n,ar);
            return 0;
        }
        else if(simple_path(dir_avan)){
            if(go_straight(&tremaux,dir_avan,m,n,ar))
                return 0;
            else if(go_left(&tremaux,dir_avan,m,n,ar))
                return 0;
            else if(go_right(&tremaux,dir_avan,m,n,ar))
                return 0;
        }
        else{
            int selec[4]={(dir_avan&1),(dir_avan&2),(dir_avan&4),(dir_avan&8)};
            int random=rand()%4;
            while(selec[random]==0)
                random=rand()%4;
            dir_avan=selec[random];
            if(go_straight(&tremaux,dir_avan,m,n,ar))
                return 0;
            else if(go_left(&tremaux,dir_avan,m,n,ar))
                return 0;
            else if(go_right(&tremaux,dir_avan,m,n,ar))
                return 0;
            else
                go_back(&tremaux,m,n,ar);
            return 0;
        }
    }
}
/*
void run(){
    int i=0;
    while(i<10000){
        tremaux_alg(2*BaseF+1,2*BaseC+1,&ArPant);
        i++;
    }
}
*/
void run(){
    int i=0;
    while(!pledge_alg(2*BaseF+1,2*BaseC+1,&ArPant)){
        i++;
    }
}
