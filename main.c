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

int RT_raton[4194304]={0};//matr1
int RT_pledge[4194304]={0};//matr2
int RT_tremaux[4194304]={0};//matr3
int RT_fattah[4194304]={0};//matr4
int ini_i,ini_j,ext_i,ext_j;
int ready=0;
int MD_best=0;

struct subject{//guarda el avanze sobre el laberinto
    int i;//posicion
    int j;
    int dir;//8 up, 4 left , 2 down, 1 right, 0 stay
    int matr; //matriz asosiada
    int sum_ang;
    int fordw;  //via avanzar
    int rev;    //via retroceder
    int r;  //colores
    int g;
    int b;
}raton,mano_izq,mano_der,pledge,tremaux,fattah;

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
    int dir=0;
    if(ini_i==0){
        dir=2;
    }
    else if(ini_i==2*BaseF){
        dir=8;
    }
    else if(ini_j==0){
        dir=1;
    }
    else{
        dir=4;
    }
    raton.dir=mano_izq.dir=mano_der.dir=pledge.dir=tremaux.dir=fattah.dir=dir;
    raton.sum_ang=mano_izq.sum_ang=mano_der.sum_ang=pledge.sum_ang=tremaux.sum_ang=fattah.sum_ang=0;
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
    fattah.fordw=64;
    fattah.rev=128;
    raton.i=mano_izq.i=mano_der.i=pledge.i=tremaux.i=fattah.i=ini_i;
    raton.j=mano_izq.j=mano_der.j=pledge.j=tremaux.j=fattah.j=ini_j;
    raton.matr=1;//matrices asosiadas
    pledge.matr=2;
    tremaux.matr=3;
    fattah.matr=4;
    mano_der.matr=0;
    mano_izq.matr=-1;
}

int MD(int i1, int j1,int i2,int j2){
    int i=i1-i2;
    int j=j1-j2;
    if (i<0)
        i=0-i;
    if (j<0)
        j=0-j;
    return i+j;
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

int dir_avanc_0(struct subject *s,int m,int n, int ar[][n]){
    int resp=0;
    if(ar[s->i][s->j+1]!=0&&((s->i!=ini_i)||(s->j+1!=ini_j))&&!((s->rev&ar[s->i][s->j+1])||(s->fordw&ar[s->i][s->j+1])))
        resp=1;
    if(ar[s->i+1][s->j]!=0&&((s->i+1!=ini_i)||(s->j!=ini_j))&&!((s->rev&ar[s->i+1][s->j])||(s->fordw&ar[s->i+1][s->j])))
        resp+=2;
    if(ar[s->i][s->j-1]!=0&&((s->i!=ini_i)||(s->j-1!=ini_j))&&!((s->rev&ar[s->i][s->j-1])||(s->fordw&ar[s->i][s->j-1])))
        resp+=4;
    if(ar[s->i-1][s->j]!=0&&((s->i-1!=ini_i)||(s->j!=ini_j))&&!((s->rev&ar[s->i-1][s->j])||(s->fordw&ar[s->i-1][s->j])))
        resp+=8;
    return resp;
}
int dir_avanc_1(struct subject *s,int m,int n, int ar[][n]){
    int resp=0;
    if(ar[s->i][s->j+1]!=0&&((s->i!=ini_i)||(s->j+1!=ini_j))&&(!(s->rev&ar[s->i][s->j+1])||!(s->fordw&ar[s->i][s->j+1])))
        resp=1;
    if(ar[s->i+1][s->j]!=0&&((s->i+1!=ini_i)||(s->j!=ini_j))&&(!(s->rev&ar[s->i+1][s->j])||!(s->fordw&ar[s->i+1][s->j])))
        resp+=2;
    if(ar[s->i][s->j-1]!=0&&((s->i!=ini_i)||(s->j-1!=ini_j))&&(!(s->rev&ar[s->i][s->j-1])||!(s->fordw&ar[s->i][s->j-1])))
        resp+=4;
    if(ar[s->i-1][s->j]!=0&&((s->i-1!=ini_i)||(s->j!=ini_j))&&(!(s->rev&ar[s->i-1][s->j])||!(s->fordw&ar[s->i-1][s->j])))
        resp+=8;
    return resp;
}

void mat_inc(int i,int j,int n,int mat[][n]){
    mat[i][j]++;
}

void mat_zero(int m,int n,int mat[][n]){
    int i,j;
    for(i=0;i<m;i++)
        for(j=0;j<m;j++)
            mat[i][j]=0;
}

int mat_read(int i,int j,int n,int mat[][n]){
    return mat[i][j];
}

void reg_mat(int i,int j,struct subject *s){//llamar con i/2,j/2 de casilla del laberinto
    if(s->matr==1)
        mat_inc(i,j,BaseC,&RT_raton);
    else if(s->matr==2)
        mat_inc(i,j,BaseC,&RT_pledge);
    else if(s->matr==3)
        mat_inc(i,j,BaseC,&RT_tremaux);
    else if(s->matr==4)
        mat_inc(i,j,BaseC,&RT_fattah);
}

int obt_mat(int i,int j,struct subject *s){//llamar con i/2,j/2 de casilla del laberinto
    int resp=0;
    if(s->matr==1)
        resp=mat_read(i,j,BaseC,&RT_raton);
    else if(s->matr==2)
        resp=mat_read(i,j,BaseC,&RT_pledge);
    else if(s->matr==3)
        resp=mat_read(i,j,BaseC,&RT_tremaux);
    else if(s->matr==4)
        resp=mat_read(i,j,BaseC,&RT_fattah);
    else if(s->matr==-1)
        resp=4;
    return resp;
}

void rest_mat(){//llamar al final de terminar solucion
        mat_zero(BaseF,BaseC,&RT_raton);
        mat_zero(BaseF,BaseC,&RT_pledge);
        mat_zero(BaseF,BaseC,&RT_tremaux);
        mat_zero(BaseF,BaseC,&RT_fattah);
}

int go_right(struct subject *s,int dir,int m,int n, int ar[][n]){
    if(s->dir!=dir){
        int resdir=s->dir;
        int i=s->i;
        int j=s->j;
        if(s->dir==8&&(dir&1)){
            j+=1;
            resdir=1;
        }else if(s->dir==4&&(dir&8)){
            i-=1;
            resdir=8;
        }else if(s->dir==2&&(dir&4)){
            j-=1;
            resdir=4;
        }else if(s->dir==1&&(dir&2)){
            i+=1;
            resdir=2;
        }else
            return 0;
        s->j=j;
        s->i=i;
        reg_mat(i/2,j/2,s);
        s->dir=resdir;
        s->sum_ang+=90;
        if(s->dir&8||s->dir&4){
            if(!(ar[s->i][s->j]&s->rev))
                ar[s->i][s->j]+=s->rev;
        }
        else{
            if(!(ar[s->i][s->j]&s->fordw))
                ar[s->i][s->j]+=s->fordw;
        }
        return 1;
    }
    return 0;
}
int go_left(struct subject *s,int dir,int m,int n, int ar[][n]){
    if(s->dir!=dir){
        int resdir=s->dir;
        int i=s->i;
        int j=s->j;
        if(s->dir==8&&(dir&4)){
            j-=1;
            resdir=4;
        }else if(s->dir==4&&(dir&2)){
            i+=1;
            resdir=2;
        }else if(s->dir==2&&(dir&1)){
            j+=1;
            resdir=1;
        }else if(s->dir==1&&(dir&8)){
            i-=1;
            resdir=8;
        }else
            return 0;
        s->j=j;
        s->i=i;
        s->dir=resdir;
        s->sum_ang-=90;
        if(s->dir&8||s->dir&4){
            if(!(ar[s->i][s->j]&s->rev))
                ar[s->i][s->j]+=s->rev;
        }
        else{
            if(!(ar[s->i][s->j]&s->fordw))
                ar[s->i][s->j]+=s->fordw;
        }
        return 1;
    }
    return 0;
}
int go_straight(struct subject *s,int dir,int m,int n, int ar[][n]){
    if(s->dir&dir){
        int i=s->i;
        int j=s->j;
        if(s->dir==8){
            i-=1;
        }else if(s->dir==4){
            j-=1;
        }else if(s->dir==2){
            i+=1;
        }else if(s->dir==1){
            j+=1;
        }else
            return 0;
        s->j=j;
        s->i=i;
        if(s->dir&8||s->dir&4){
            if(!(ar[s->i][s->j]&s->rev))
                ar[s->i][s->j]+=s->rev;
        }
        else{
            if(!(ar[s->i][s->j]&s->fordw))
                ar[s->i][s->j]+=s->fordw;
        }
        return 1;
    }
    return 0;
}
void go_back(struct subject *s,int m,int n, int ar[][n]){
    int i=s->i;
    int j=s->j;
    if(s->dir==8){
        s->dir=2;
        i+=1;
    }else if(s->dir==4){
        s->dir=1;
        j+=1;
    }else if(s->dir==2){
        s->dir=8;
        i-=1;
    }else if(s->dir==1){
        s->dir=4;
        j-=1;
    }
    s->j=j;
    s->i=i;
    if(s->sum_ang<0)
        s->sum_ang-=180;
    else
        s->sum_ang+=180;
    if(s->dir&8||s->dir&4){
        if(!(ar[s->i][s->j]&s->rev))
            ar[s->i][s->j]+=s->rev;
        }
    else{
        if(!(ar[s->i][s->j]&s->fordw))
            ar[s->i][s->j]+=s->fordw;
        }
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
            if(go_left(&mano_izq,dir_avan,m,n,ar)){
                return 0;
                }
            else if(go_straight(&mano_izq,dir_avan,m,n,ar)){
                return 0;
                }
            else if(go_right(&mano_izq,dir_avan,m,n,ar)){
                return 0;
                }
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
                if(go_straight(&pledge,dir_avan,m,n,ar))
                    return 0;
                else if(go_left(&pledge,dir_avan,m,n,ar))
                    return 0;
                else if(go_right(&pledge,dir_avan,m,n,ar))
                    return 0;
                return 0;
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

int tremaux_alg(int m,int n, int ar[][n]){ //marcas=8192
    if(tremaux.i==ext_i&&tremaux.j==ext_j)
        return 1;
    else{
        int dir_avan=dir_avanc(&tremaux,m,n,ar);
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
            dir_avan=dir_avanc_0(&tremaux,m,n,ar);
            int selec[4]={(dir_avan&1),(dir_avan&2),(dir_avan&4),(dir_avan&8)};
            int random=rand()%4;
            int i=0;
            while(selec[random]==0&&i<30){
                random=rand()%4;
                i++;
                }
            dir_avan=selec[random];
            if(go_straight(&tremaux,dir_avan,m,n,ar)){
                return 0;
            }
            else if(go_left(&tremaux,dir_avan,m,n,ar)){
                return 0;
            }
            else if(go_right(&tremaux,dir_avan,m,n,ar)){
                return 0;
            }
            else{
                dir_avan=dir_avanc_1(&tremaux,m,n,ar);
                int selec[4]={(dir_avan&1),(dir_avan&2),(dir_avan&4),(dir_avan&8)};
                random=rand()%4;
                i=0;
                while(selec[random]==0&&i<30){
                    random=rand()%4;
                    i++;
                    }
                dir_avan=selec[random];
                if(go_straight(&tremaux,dir_avan,m,n,ar)){
                    return 0;
                }
                else if(go_left(&tremaux,dir_avan,m,n,ar)){
                    return 0;
                }
                else if(go_right(&tremaux,dir_avan,m,n,ar)){
                    return 0;
                }
                else{
                    go_back(&tremaux,m,n,ar);
                }
            }
            return 0;
        }
    }
}

/*Heuristica Manhatan -> H = Math.Abs(nodoActual.X – nodoFinal.X) + Math.Abs(nodoActual.Y – nodoFinal.Y)
int fattah_alg(int m,int n, int ar[][n]){
    if(fattah.i!=ext_i||fattah.j!=ext_j){
        if(there exists a productive path){
            Take the productive path;
        }else{
            MD_best = MD(cur, dst);
            Imagine a line between cur and dst;
            Take the first path in the left/right of the line;// The left/right selection affects the following hand rule
            while(MD(cur, dst) != MD_best || there does not exist a productive path){
                Follow the right-hand/left-hand rule;// The opposite of the selected side of the line
            }
        }
        return 0;
    }
    else
        return 1;
}



void run(){
    int i=0;
    while(i<1000){
        tremaux_alg(2*BaseF+1,2*BaseC+1,&ArPant);
        i++;
    }
}
*/
void run(){
    int i=0;
    while(!tremaux_alg(2*BaseF+1,2*BaseC+1,&ArPant)){
        i++;
    }
}

void draw_path(struct subject *s,int pos_x,int pos_y,int wight,int higth,int m,int n, int ar[][n]){
    //no pintar otra vez, pintar encima añadiendo
    //mostrar parte,no todo, pintar en superficie 12 veces mas grande que parte
}

