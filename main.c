#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <unistd.h> //nanosleep(&ps, NULL); struct timespec ps;

int ArBase[4194304]={0};
int ArPant[16793604]={0};
int BaseF=1;
int BaseC=1;
int Entry=0;
int Exit=0;
int ready=0;
int maxd=0;

int RT_raton[4194304]={0};//matr1
int RT_pledge[4194304]={0};//matr2
int RT_tremaux[4194304]={0};//matr3
int RT_fattah[4194304]={0};//matr4
int ini_i,ini_j,ext_i,ext_j;
int MD_best=0;

int readyRA=0;
int readyMD=0;
int readyMI=0;
int readyAP=0;
int readyAT=0;
int readyAF=0;
int SolReady=0;
int RunSol=0;
int MazeOn=0;

int solRA=0;
int solMD=0;
int solMI=0;
int solAP=0;
int solAT=0;
int solAF=0;

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

GtkDialog* mnsjResolv;
GtkDialog* Generar;
GtkDialog* Confirm;
GtkApplicationWindow* DAdial;
GtkDrawingArea* DrawArea;
GtkFileChooserDialog* DA;
GtkFileChooserDialog* DG;
GtkAdjustment *adjustmentf;
GtkAdjustment *adjustmentc;
GtkWidget* window;
GtkFileChooser* ABRIR;
GtkFileChooser* GUARDAR;
GtkSpinButton* filas;
GtkSpinButton* columnas;
GtkButton* GuardarB;
GtkSwitch* switchB;

GtkDialog* Cont_casillas;
GtkCheckButton* chkRA;
GtkCheckButton* chkMD;
GtkCheckButton* chkMI;
GtkCheckButton* chkAP;
GtkCheckButton* chkAT;
GtkCheckButton* chkAF;
GtkLabel* ral;
GtkLabel* mdl;
GtkLabel* mil;
GtkLabel* apl;
GtkLabel* atl;
GtkLabel* afl;
GtkButton* BEjeSol;
GtkButton* salireje;
GtkToggleButton* pausar;



int corriendo=0;
int solucion=1;
int activo=0;
int lst;
int scale=1;
int mejorar=0;
int anti,antj,ii,ij,tamC;

int eab=0;

char *TRes;

static gint pos_x = 0;
static gint pos_y = 0;
static gint pos2_x = 0;
static gint pos2_y = 0;
static gint hg = 5; 
static gint wd = 5; 
static gint Thg = 5; 
static gint Twd = 5;
static int depth = 0;
static cairo_surface_t * completo=NULL;
static cairo_surface_t * segment=NULL;
static cairo_t *aux=NULL;

int guardar();
int abrir();
void Pasar();
void configspnbttn();

void set_basef();
void set_basec();
void calcular_d();
static void do_drawing();
static void do_segdrawing(cairo_t *cr,int x, int y,int w, int h,int m,int n,int mat[][n]);

int resolv(int m,int n,int ar[][n]);
void addFrontier(int m, int n,int i, int j,int ar[][n]);
void removeBarrier(int m, int n,int i, int j,int ar[][n]);
void setentrys(int m, int n,int ar[][n]); //n m > 1
void generate_maze();
void desplegar ();
int abrir_maze(char* archivo);
void guardar_maze(char* archivo);
static void clear_surface ();
static void do_bdrawing(cairo_t *cr);
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data);
static gboolean check_escape(GtkWidget *widget, GdkEventKey *event, gpointer data);
//
void setij();
int random_mouse(int m,int n, int ar[][n]);
void run();
void correrSoluciones();


int main(int argc, char *argv[])
{
    srand(time(NULL));
    
    GtkBuilder      *builder;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "inter.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));

	TRes= g_new (gchar,20);

	mnsjResolv=GTK_DIALOG(gtk_builder_get_object(builder, "msj_Resolver"));
	DAdial=GTK_APPLICATION_WINDOW(gtk_builder_get_object(builder, "Maze_area"));
	DrawArea=GTK_DRAWING_AREA(gtk_builder_get_object(builder, "drawArea"));
	Generar=GTK_DIALOG(gtk_builder_get_object(builder, "msj_Generar"));
	Confirm=GTK_DIALOG(gtk_builder_get_object(builder, "Confirm"));
	filas=GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbuttonFilas"));
    columnas=GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbuttonColumnas"));
    DA=GTK_FILE_CHOOSER_DIALOG(gtk_builder_get_object(builder,"fileAbrir"));
    DG=GTK_FILE_CHOOSER_DIALOG(gtk_builder_get_object(builder,"fileGuardar"));
    GuardarB=GTK_BUTTON(gtk_builder_get_object(builder,"GuardarB"));
    switchB=GTK_SWITCH (gtk_builder_get_object(builder,"switch"));
    
    Cont_casillas=GTK_DIALOG(gtk_builder_get_object(builder,"Cont_casillas"));
	chkRA=GTK_CHECK_BUTTON(gtk_builder_get_object(builder,"chkRA"));
	chkMD=GTK_CHECK_BUTTON(gtk_builder_get_object(builder,"chkMD"));
	chkMI=GTK_CHECK_BUTTON(gtk_builder_get_object(builder,"chkMI"));
	chkAP=GTK_CHECK_BUTTON(gtk_builder_get_object(builder,"chkAP"));
	chkAT=GTK_CHECK_BUTTON(gtk_builder_get_object(builder,"chkAT"));
	chkAF=GTK_CHECK_BUTTON(gtk_builder_get_object(builder,"chkAF"));
	ral=GTK_LABEL(gtk_builder_get_object(builder,"ral"));
	mdl=GTK_LABEL(gtk_builder_get_object(builder,"mdl"));
	mil=GTK_LABEL(gtk_builder_get_object(builder,"mil"));
	apl=GTK_LABEL(gtk_builder_get_object(builder,"apl"));
	atl=GTK_LABEL(gtk_builder_get_object(builder,"atl"));
	afl=GTK_LABEL(gtk_builder_get_object(builder,"afl"));
	BEjeSol=GTK_BUTTON(gtk_builder_get_object(builder,"BEjeSol"));
	salireje=GTK_BUTTON(gtk_builder_get_object(builder,"salireje"));
	pausar=GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"pausar"));
    
    ABRIR=GTK_FILE_CHOOSER(DA);
	GUARDAR=GTK_FILE_CHOOSER(DG);
    gtk_builder_connect_signals(builder, NULL);
    configspnbttn();
    gtk_widget_set_sensitive (GTK_WIDGET(GuardarB), FALSE);

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();
    /*
    BaseF=5;
	BaseC=6;//n m > 1
    if(resolv(BaseF,BaseC,&ArBase))
        generate_maze();
    guardar_maze("prueba.txt");
    desplegar();
    printf("\n \n");
    if (abrir_maze("prueba.txt"))
        desplegar();*/
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

void resetMazeMat(){
	int i,j;
	for(i =0; i<(BaseF*2+1);i++){
        for(j =0; j<(BaseC*2+1);j++){
			ArPant[i*(BaseC*2+1)+j]=0;
		}
	}
}

void restaurMazeMat(){
	int i,j;
	for(i =0; i<(BaseF*2+1);i++){
        for(j =0; j<(BaseC*2+1);j++){
			if (ArPant[i*(BaseC*2+1)+j]!=0)
				ArPant[i*(BaseC*2+1)+j]=1;
		}
	}
}

void generate_maze(){
    int t,i,j,n,act,E,S,ex;
    n=act=0;
    resetMazeMat();
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
    restaurMazeMat();
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
        for(j=0;j<n;j++)
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
        return 0;
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
        return 0;
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
        return 0;
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
        return 0;
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
        return 0;
    }
}

/*Heuristica Manhatan -> H = Math.Abs(nodoActual.X-nodoFinal.X) + Math.Abs(nodoActual.Y - nodoFinal.Y)
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





//----------------------------------------------------------------
void calcular_d(){
	Thg=Twd=hg=wd=5*scale;
	pos2_x=pos_x=pos2_y=pos_y=0;
	ii=ij=0;
	tamC=3;
	depth=0;
	int max=BaseC;
	if (BaseC<BaseF)
		max=BaseF;
	maxd=0;
	while(4*scale*maxd+3*scale<(max*2*scale+scale)){
		maxd++;
	}
}

int range(int n,int w, int m){
	if (n<=0)
		return 0;
	else if(m<w)
		return 0;
	else if (m<n+w)
		return m-w;
	else
		return n;
}

void zoomIn(){
	if (depth>0){
		depth--;
		tamC=4*depth+3;
		ii+=2;
		ij+=2;
		hg=wd=4*scale*depth+3*scale;
		pos_x=pos_x+2*scale;
		pos_y=pos_y+2*scale;
	}
}

void zoomOut(){
	int max;
	if (depth<maxd){
		depth++;
		if(depth<50){
			max=BaseF;
			if(max<BaseC)
				max=BaseC;
			tamC=range(4*depth+3,0,2*max+1);
			ii=range(ii-2,tamC,2*BaseF+1);
			ij=range(ij-2,tamC,2*BaseC+1);
		}
		else{
			hg=wd=4*scale*depth+3*scale;
			pos_x=range(pos_x-2*scale,wd,Twd);
			pos_y=range(pos_y-2*scale,hg,Thg);
		}
	}else{
		max=Twd;
		if(Twd<Thg)
			max=Thg;
		if(depth>49)
			hg=wd=max;
		else{
			max=BaseF;
			if(max<BaseC)
				max=BaseC;
			tamC=2*max+1;
		}
		ii=0;
		ij=0;
		pos_x=0;
		pos_y=0;
	}
}

void desplazar(gdouble x, gdouble y,gint h, gint w){
	if (depth<maxd){
		if(depth<50){
			if(pos2_y<y)
				ii=range(ii-2,tamC,2*BaseF+1);
			else if (pos2_y>y)
				ii=range(ii+2,tamC,2*BaseF+1);
			if(pos2_x<x)
				ij=range(ij-2,tamC,2*BaseC+1);
			else if (pos2_x>x)
				ij=range(ij+2,tamC,2*BaseC+1);
			}
		else{
			pos_x=range(pos_x+(pos2_x - x),wd,Twd);
			pos_y=range(pos_y+(pos2_y - y),hg,Thg);
		}
		pos2_x=x;
		pos2_y=y;
	}
}

//---------------------------------------------------------------resolucion_dibujo

int way_log_2(int n){
	int i=1;
	while (i<14&&pow(2,i)!=n)
		i++;
	return i-1;
}

int activ_path(int ford,int rev,int i,int j,int n,int mat[][n]){
	int t=0;
	if (mat[i][j+1]&ford)
		t+=2;
	if (mat[i][j+1]&rev)
		t++;
	if (mat[i+1][j]&ford)
		t+=8;
	if (mat[i+1][j]&rev)
		t+=4;
	if (mat[i][j-1]&ford)
		t+=16;
	if (mat[i][j-1]&rev)
		t+=32;
	if (mat[i-1][j]&ford)
		t+=64;
	if (mat[i-1][j]&rev)
		t+=128;
	return t;
}

int n_path(int av,int dir){//dir 0=total,8 up,2 down,4 left,1 right,path 88,44,22,11: EntrSal
	int t=0;
	if(!dir){
		if(av&1)
			t++;
		if(av&2)
			t++;
		if(av&4)
			t++;
		if(av&8)
			t++;
		if(av&16)
			t++;
		if(av&32)
			t++;
		if(av&64)
			t++;
		if(av&128)
			t++;
	}else if (dir&1){
		if(av&1)
			t++;
		if(av&2)
			t++;
	}else if (dir&2){
		if(av&4)
			t++;
		if(av&8)
			t++;
	}else if (dir&4){
		if(av&16)
			t++;
		if(av&32)
			t++;
	}else if (dir&8){
		if(av&64)
			t++;
		if(av&128)
			t++;
	}
	return t;
}

void uturn(cairo_t *cr,int path,int n_via,int i,int j,int n,int mat[][n],struct subject *s){//sujeto tiene color, n via es carril que le toca
	cairo_set_source_rgb(cr, s->r, s->g, s->b);
	if (path&1){
		cairo_rectangle(cr, n_via, n_via,		12-n_via, 1);
		cairo_rectangle(cr, n_via, n_via+1,		1, 10-2*n_via);
		cairo_rectangle(cr, n_via, 11-n_via,	12-n_via, 1);
	}
	else if (path&2){
		cairo_rectangle(cr, n_via, n_via,		1, 12-n_via);
		cairo_rectangle(cr, n_via+1, n_via,		10-2*n_via, 1);
		cairo_rectangle(cr, 11-n_via, n_via,	1, 12-n_via);
	}
	else if (path&4){
		cairo_rectangle(cr, 0, n_via,			12-n_via, 1);
		cairo_rectangle(cr, 11-n_via, n_via+1,	1, 10-2*n_via);
		cairo_rectangle(cr, 0, 11-n_via,		12-n_via, 1);
	}
	else if (path&8){
		cairo_rectangle(cr, n_via, 0,			1, 12-n_via);
		cairo_rectangle(cr, n_via+1, 11-n_via,	10-2*n_via, 1);
		cairo_rectangle(cr, 11-n_via, 0,		1, 12-n_via);
	}
	cairo_set_source_rgb(cr, 0, 0, 0);
}

void strln(cairo_t *cr,int path,int n_via,int i,int j,int n,int mat[][n],int act,struct subject *s){//sujeto tiene color
	cairo_set_source_rgb(cr, s->r, s->g, s->b);
	if (path&1&&path&4){
		if((act&1)&&(mat[i][j-1]&s->fordw&&mat[i][j+1]&s->fordw)){
			cairo_rectangle(cr, 0, n_via,		12, 1);
			}
		if((act&4)&&(mat[i][j-1]&s->rev&&mat[i][j+1]&s->rev)){
			cairo_rectangle(cr, 0, 11-n_via,	12, 1);
			}
	}
	if (path&2&&path&8){
		if((act&2)&&(mat[i-1][j]&s->rev&&mat[i+1][j]&s->rev)){
			cairo_rectangle(cr, 11-n_via, 0,	1, 12);
			}
		if((act&8)&&(mat[i-1][j]&s->fordw&&mat[i+1][j]&s->fordw)){
			cairo_rectangle(cr, n_via, 0,		1, 12);
			}
	}
	cairo_set_source_rgb(cr, 0, 0, 0);
}

void ltrln(cairo_t *cr,int path,int n_via,int i,int j,int n,int mat[][n],int act,struct subject *s){//sujeto tiene color
	cairo_set_source_rgb(cr, s->r, s->g, s->b);
	if ((path&1&&path&2&&act&4)&&(mat[i+1][j]&s->rev&&mat[i][j+1]&s->rev)){
		cairo_rectangle(cr, 11-n_via, 11-n_via,	1+n_via, 1);
		cairo_rectangle(cr, 11-n_via, 11-n_via,	1, 1+n_via);
	}
	if ((path&2&&path&4&&act&8)&&(mat[i+1][j]&s->fordw&&mat[i][j-1]&s->rev)){
		cairo_rectangle(cr, 0, 11-n_via,		1+n_via, 1);
		cairo_rectangle(cr, n_via, 11-n_via,	1, 1+n_via);
	}
	if ((path&4&&path&8&&act&1)&&(mat[i-1][j]&s->fordw&&mat[i][j-1]&s->fordw)){
		cairo_rectangle(cr, 0, n_via,			1+n_via, 1);
		cairo_rectangle(cr, n_via, 0,			1, 1+n_via);
	}
	if ((path&8&&path&1&&act&2)&&(mat[i][j+1]&s->fordw&&mat[i-1][j]&s->rev)){
		cairo_rectangle(cr, 11-n_via, n_via,	1+n_via, 1);
		cairo_rectangle(cr, 11-n_via, 0,		1, 1+n_via);
	}
	cairo_set_source_rgb(cr, 0, 0, 0);
}

void rtrln(cairo_t *cr,int path,int n_via,int i,int j,int n,int mat[][n],int act,struct subject *s){//sujeto tiene color
	cairo_set_source_rgb(cr, s->r, s->g, s->b);
	if ((path&1&&path&8&&act&4)&&(mat[i-1][j]&s->fordw&&mat[i][j+1]&s->rev)){
		cairo_rectangle(cr, n_via, 11-n_via,	11-n_via, 1);
		cairo_rectangle(cr, n_via, 0,			1, 11-n_via);
	}
	if ((path&2&&path&1&&act&8)&&(mat[i+1][j]&s->fordw&&mat[i][j+1]&s->fordw)){
		cairo_rectangle(cr, n_via, n_via,		11-n_via, 1);
		cairo_rectangle(cr, n_via, n_via,		1, 11-n_via);
	}
	if ((path&4&&path&2&&act&1)&&(mat[i][j-1]&s->fordw&&mat[i+1][j]&s->rev)){
		cairo_rectangle(cr, 0, n_via,			11-n_via, 1);
		cairo_rectangle(cr, 11-n_via, n_via,	1, 11-n_via);
	}
	if ((path&8&&path&4&&act&2)&&(mat[i-1][j]&s->rev&&mat[i][j-1]&s->rev)){
		cairo_rectangle(cr, 0, 11-n_via,		11-n_via, 1);
		cairo_rectangle(cr, 11-n_via, 0,		1, 11-n_via);
	}
	cairo_set_source_rgb(cr, 0, 0, 0);
}

int paths(int i,int j,int n,int mat[][n]){
	int t =15;
	if (mat[i-1][j]==0)
		t-=8;
	if (mat[i][j-1]==0)
		t-=4;
	if (mat[i+1][j]==0)
		t-=2;
	if (mat[i][j+1]==0)
		t-=1;
	return t;
}

int num_paths(int path){
	int t=0;
	if (path&1)
		t++;
	if (path&2)
		t++;
	if (path&4)
		t++;
	if (path&8)
		t++;
	return t;
}

void reg_path(cairo_t *cr,int i,int j,int n,int mat[][n],struct subject *s){//sujeto tiene color struct subject *s,ford y rev fromsubject
	int vias,n_via,num_vias,act_path,tot_v;
	vias=paths(i,j,n,mat);
	num_vias=num_paths(vias);
	act_path=activ_path(s->fordw,s->rev,i,j,n,mat);//ford y rev fromsubject
	if ((mat[i][j]&s->fordw)||(mat[i][j]&s->rev))
		n_via=way_log_2(s->fordw);
	if(num_vias==1)
		uturn(cr,vias,n_via,i,j,n,mat,s);//falta subject
	else if(num_vias==2){
		strln(cr,vias,n_via,i,j,n,mat,15,s);
		ltrln(cr,vias,n_via,i,j,n,mat,15,s);
		rtrln(cr,vias,n_via,i,j,n,mat,15,s);
	}
	else if(num_vias==3){
		tot_v=n_path(act_path,0);
		if(tot_v==6){// todo lleno
			if(1){//get rigt turns from mat i/2,j/2 > 0
				rtrln(cr,vias,n_via,i,j,n,mat,15,s);
				strln(cr,vias,n_via,i,j,n,mat,15,s);
			}else{
				ltrln(cr,vias,n_via,i,j,n,mat,15,s);
				strln(cr,vias,n_via,i,j,n,mat,15,s);
			}
		}else if(tot_v==4){
			if((n_path(act_path,1)==2&&n_path(act_path,4)==2)||(n_path(act_path,2)==2&&n_path(act_path,8)==2)){
				strln(cr,vias,n_via,i,j,n,mat,15,s);
			}
			else if(n_path(act_path,1)==0||n_path(act_path,4)==0||n_path(act_path,2)==0||n_path(act_path,8)==0){
				ltrln(cr,vias,n_via,i,j,n,mat,15,s);
				rtrln(cr,vias,n_via,i,j,n,mat,15,s);
			}
			else{
				if(1){//get rigt turns from mat i/2,j/2 > 0
					rtrln(cr,vias,n_via,i,j,n,mat,15,s);
					strln(cr,vias,n_via,i,j,n,mat,15,s);
				}else{
					ltrln(cr,vias,n_via,i,j,n,mat,15,s);
					strln(cr,vias,n_via,i,j,n,mat,15,s);
				}
			}
		}else{
			strln(cr,vias,n_via,i,j,n,mat,15,s);
			ltrln(cr,vias,n_via,i,j,n,mat,15,s);
			rtrln(cr,vias,n_via,i,j,n,mat,15,s);
		}
	}
	else if(num_vias==4){
		tot_v=n_path(act_path,0);
		if(tot_v==8){
			if(1){//get rigt turns from mat i/2,j/2 > 0
				rtrln(cr,vias,n_via,i,j,n,mat,15,s);
			}else{
				ltrln(cr,vias,n_via,i,j,n,mat,15,s);
			}
		}
		else if(tot_v==6){
			if(n_path(act_path,1)==0||n_path(act_path,4)==0||n_path(act_path,2)==0||n_path(act_path,8)==0){
				if((n_path(act_path,1)==2&&n_path(act_path,4)==2)||(n_path(act_path,2)==2&&n_path(act_path,8)==2)){
					strln(cr,vias,n_via,i,j,n,mat,15,s);
				}
				else{
					if(1){//get rigt turns from mat i/2,j/2 > 0
						rtrln(cr,vias,n_via,i,j,n,mat,15,s);
						strln(cr,vias,n_via,i,j,n,mat,15,s);
					}else{
						ltrln(cr,vias,n_via,i,j,n,mat,15,s);
						strln(cr,vias,n_via,i,j,n,mat,15,s);
					}
				}
			}
		}
		else if(tot_v==4){
			if((n_path(act_path,1)==2&&n_path(act_path,4)==2)||(n_path(act_path,2)==2&&n_path(act_path,8)==2)){
				strln(cr,vias,n_via,i,j,n,mat,15,s);
			}
			else if(n_path(act_path,1)==1||n_path(act_path,4)==1||n_path(act_path,2)==1||n_path(act_path,8)==1){
				if(1){//get rigt turns from mat i/2,j/2 > 0
					rtrln(cr,vias,n_via,i,j,n,mat,15,s);
					strln(cr,vias,n_via,i,j,n,mat,15,s);
				}else{
					ltrln(cr,vias,n_via,i,j,n,mat,15,s);
					strln(cr,vias,n_via,i,j,n,mat,15,s);
				}
			}else{
				ltrln(cr,vias,n_via,i,j,n,mat,15,s);
				rtrln(cr,vias,n_via,i,j,n,mat,15,s);
			}
		}
		else{
			if(1){//get rigt turns from mat i/2,j/2 > 0
				rtrln(cr,vias,n_via,i,j,n,mat,15,s);
				strln(cr,vias,n_via,i,j,n,mat,15,s);
			}else{
				ltrln(cr,vias,n_via,i,j,n,mat,15,s);
				strln(cr,vias,n_via,i,j,n,mat,15,s);
			}
		}
	}
}

//--------------------------------------------------------------------------

void on_window_main_destroy()
{
    gtk_main_quit();
}

static gboolean check_escape(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval == GDK_KEY_Escape) {
		gtk_widget_hide(GTK_WIDGET(DAdial));
		corriendo=0;
		activo=0;
		MazeOn=0;
		gtk_widget_set_sensitive (GTK_WIDGET(GuardarB), FALSE);
		return TRUE;
	}
	return FALSE;
}

void on_SalirB_clicked()
{
	gtk_main_quit();
}

void on_GenerarB_clicked()
{
	if(!activo){
		if (!corriendo){
			corriendo=1;
			activo=1;
			gtk_dialog_run(Generar);
		}
	}
}

void on_GenAceptar_clicked(){
	Pasar();
	if(resolv(BaseF,BaseC,&ArBase))
            generate_maze();
    calcular_d();
    if (completo)
		clear_surface ();
	gtk_widget_show(GTK_WIDGET(DAdial));
	MazeOn=1;
    gtk_widget_hide(GTK_WIDGET(Generar));
}

void on_GenCancelar_clicked(){
	corriendo=0;
	activo=0;
	gtk_widget_hide(GTK_WIDGET(Generar));
}

void on_ResolverB_clicked()
{
	if((!RunSol)&&MazeOn){
		gtk_dialog_run(mnsjResolv);
		RunSol=1;
	}
}

void on_AbrirB_clicked()
{
	if(!corriendo && !activo){
		gtk_dialog_run(GTK_DIALOG(DA));
		}
}

void on_GuardarB_clicked()
{
	if(corriendo&&solucion)
		gtk_dialog_run(GTK_DIALOG(DG));
}

void on_CGAceptar_clicked(){
	eab=1;
	gtk_widget_hide(GTK_WIDGET(Confirm));
}

void on_CGCancelar_clicked(){
	gtk_widget_hide(GTK_WIDGET(Confirm));
}

void on_BGGuardar_clicked()
{
    archivo= gtk_file_chooser_get_current_name (GTK_FILE_CHOOSER(GUARDAR));
    if( access( archivo, F_OK ) != -1 )
		gtk_dialog_run(Confirm);
	else
		eab=1;
	if (eab){
		guardar_maze(archivo);
		gtk_widget_hide(GTK_WIDGET(DG));
		eab=0;
	}
}

void on_BGCancelar_clicked()
{
	gtk_widget_hide(GTK_WIDGET(DG));
}

void on_BAAbrir_clicked()
{
	if(!(activo)){
		archivo=gtk_file_chooser_get_filename(ABRIR);
		if(abrir()){
			activo=1;
			corriendo=1;
			scale=5;
			calcular_d();
			if (completo)
				clear_surface ();
			gtk_widget_show(GTK_WIDGET(DAdial));
			MazeOn=1;
		}
		gtk_widget_hide(GTK_WIDGET(DA));
	}
}

void on_BACancelar_clicked()
{
	gtk_widget_hide(GTK_WIDGET(DA));
	activo=0;
}

void on_msj_aceptar_clicked()
{
	gtk_widget_hide(GTK_WIDGET(mnsjResolv));
	RunSol=0;
}

//_______________________________________________________________________

void on_chkMD_toggled(){
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkMD))){
		solMD=1;
	}else{
		solMD=0;
	}
}

void on_chkAF_toggled(){
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkAF))){
		solAF=1;
	}else{
		solAF=0;
	}
}

void on_chkAT_toggled(){
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkAT))){
		solAT=1;
	}else{
		solAT=0;
	}
}

void on_chkAP_toggled(){
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkAP))){
		solAP=1;
	}else{
		solAP=0;
	}
}

void on_chkMI_toggled(){
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkMI))){
		solMI=1;
	}else{
		solMI=0;
	}
}

void on_chkRA_toggled(){
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkRA))){
		solRA=1;
	}else{
		solRA=0;
	}
}

void on_BEjeSol_clicked(){
	gtk_widget_hide(GTK_WIDGET(mnsjResolv));
	gtk_dialog_run(GTK_DIALOG(Cont_casillas));
	correrSoluciones();
}

void on_salireje_clicked(){
	gtk_widget_hide(GTK_WIDGET(Cont_casillas));
	RunSol=0;
	SolReady=1;//al detener el ciclo: poner 0
}

void on_pausar_toggled(){
	if(gtk_toggle_button_get_active(pausar)){
		gtk_button_set_label(GTK_BUTTON(pausar),"Continuar");
		RunSol=0;
	}else{
		gtk_button_set_label(GTK_BUTTON(pausar),"Pausar");
		RunSol=1;
	}
}

//_______________________________________________________________________

gboolean on_Maze_area_delete_event(GtkWidget *widget, GdkEvent *event, gpointer dat)
{
	gtk_widget_hide(widget);
	corriendo=0;
	activo=0;
	MazeOn=0;
	gtk_widget_set_sensitive (GTK_WIDGET(GuardarB), FALSE);
	return TRUE;
}

static void clear_surface ()
{
  if (completo)
    cairo_surface_destroy (completo);
  Thg = 2*scale*BaseF+3*scale; 
  Twd = 2*scale*BaseC+3*scale;
  int max=Thg;
	if(max<Twd)
		max=Twd;
  completo = gdk_window_create_similar_surface (gtk_widget_get_window (GTK_WIDGET(DrawArea)),CAIRO_CONTENT_COLOR,max,max);
  cairo_t *cr;
  cr = cairo_create (completo);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr); 
  do_bdrawing(cr);
  cairo_destroy (cr);
  gtk_widget_set_sensitive (GTK_WIDGET(GuardarB), TRUE);
}

static gboolean configure_event_cb (GtkWidget *widget,GdkEventConfigure *event,gpointer data)
{
  if (completo)
    cairo_surface_destroy (completo);
  completo = gdk_window_create_similar_surface (gtk_widget_get_window (widget),CAIRO_CONTENT_COLOR,gtk_widget_get_allocated_width (widget),gtk_widget_get_allocated_height (widget));
  cairo_t *cr;
  cr = cairo_create (completo);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);
  cairo_destroy (cr);
  clear_surface ();
  if(segment)
	cairo_surface_destroy (segment);
  segment = gdk_window_create_similar_surface (gtk_widget_get_window (widget),CAIRO_CONTENT_COLOR,tamC+2,tamC+2);//286
  cr = cairo_create (segment);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint (cr);
  cairo_destroy (cr);
  return TRUE;
}

static gboolean scrollZ(GtkWidget * Widget,GdkEventScroll* event, gpointer data)
{
	if(event->direction==GDK_SCROLL_UP){
		zoomIn();
	}
	else if(event->direction==GDK_SCROLL_DOWN){
		zoomOut();
	}
	do_drawing();
	gtk_widget_queue_draw(Widget);
	return TRUE;
} 

static gboolean button_press_event_cb (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	if (event->button == GDK_BUTTON_PRIMARY){
		pos2_x=event->x;
		pos2_y=event->y;
   }
  return TRUE;
}

static void activate_cb (GObject *switcher, GParamSpec *pspec, gpointer user_data)
{
  if (gtk_switch_get_active (GTK_SWITCH (switcher)))
    mejorar=1;
  else
    mejorar=0;
}

static gboolean mover (GtkWidget *widget, GdkEventMotion *event, gpointer data){
	if (event->state & GDK_BUTTON1_MASK){
		gint width, height;
		width=gtk_widget_get_allocated_width(widget);
		height=gtk_widget_get_allocated_height(widget);
		desplazar (event->x, event->y,height,width);
    }
	do_drawing();
	gtk_widget_queue_draw(widget);
	return TRUE;
}

int inScreen(int i,int j){
	int ancho=range(j-ij-1,0,ij+tamC);
	int alto=range(i-ii-1,0,ii+tamC);
	if((ancho>0&&ancho<ij+tamC)&&(alto>0&&alto<ii+tamC))
		return 1;
	else
		return 0;
}

static void do_segdrawing(cairo_t *cr,int x, int y,int w, int h,int m,int n,int mat[][n]){
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
    /*
    tamC=2*depth+1;
    anti=antj=ii=0;
    */
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_translate(cr,12, 12);
	for(int i=0;i<h;i++){
		for(int j=0; j<w; j++){
			if(mat[y+i][x+j]==0){
				cairo_rectangle(cr, 0, 0, 12, 12);
				cairo_fill(cr); 
			} 
			else {
				if(solRA){
					reg_path(cr,y+i,x+j, n,mat,&raton);
				}
				if(solMD){
					reg_path(cr,y+i,x+j, n,mat,&mano_der);
				}
				if(solMI){
					reg_path(cr,y+i,x+j, n,mat,&mano_izq);
				}
				if(solAP){
					reg_path(cr,y+i,x+j, n,mat,&pledge);
				}
				if(solAT){
					reg_path(cr,y+i,x+j, n,mat,&tremaux);
				}
				/*if(solAF){
					reg_path(cr,y+i,x+j, n,mat,&fattah);
				}*/
			}
			cairo_translate(cr, 12, 0);
			}
		cairo_translate(cr, -1*w*12, 12);
	}
}

static void confScreen(GtkWidget* widget){
	cairo_t *cr;
	if(segment)
		cairo_surface_destroy (segment);
	segment = gdk_window_create_similar_surface (gtk_widget_get_window (widget),CAIRO_CONTENT_COLOR,12*tamC+24,12*tamC+24);//286
	cr = cairo_create (segment);
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_paint (cr);
	do_segdrawing(cr,ij,ii,tamC,tamC,2*BaseF+1,2*BaseC+1,&ArPant);
	cairo_destroy (cr);
}

static void do_drawing(){
	GtkWidget* widget=GTK_WIDGET(DrawArea);
	gint width, height;
	width=gtk_widget_get_allocated_width(widget);
	height=gtk_widget_get_allocated_height(widget);
	GdkPixbuf *alt=NULL;
	if(depth<50){
		confScreen(widget);
		cairo_set_source_surface (aux, segment, 0, 0);
		alt=gdk_pixbuf_get_from_surface(segment,0,0,12*tamC+24,12*tamC+24);
		}
	else{
		cairo_set_source_surface (aux, completo, 0, 0);
		alt=gdk_pixbuf_get_from_surface(completo,pos_x,pos_y,wd, hg);
	}
	GdkPixbuf *temp=gdk_pixbuf_scale_simple(alt, width, height, GDK_INTERP_TILES);
	gdk_cairo_set_source_pixbuf(aux, temp, 0, 0);
	cairo_paint(aux);
	g_object_unref(alt);
	g_object_unref(temp);
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{ 
	if (aux==NULL)
		aux = cr;
	do_drawing();
	return TRUE;
}


static void do_bdrawing(cairo_t *cr){
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_paint(cr);
	int l=(2*BaseF+1)*(2*BaseC+1);
    int n=(2*BaseC+1);
    int tam=scale;
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_translate(cr, scale, scale);
    for(int i = 0;i<l;i++){
        if(i%n==0&&i!=0){
            cairo_translate(cr, -1*n*tam, tam);
        }
        if(ArPant[i]==0){
			cairo_rectangle(cr, 0, 0, tam, tam);
			cairo_fill(cr); 
		}
		cairo_translate(cr, tam, 0);
    }
}

void Pasar(){
	gint value=gtk_spin_button_get_value (filas);
    BaseF=value;
    value=gtk_spin_button_get_value(columnas);
    BaseC=value;
    int max=(BaseF+BaseC)/2;
    
    if(mejorar){
		scale=range(512/max,0,256);
		scale++;
	}else
		scale=1;
}
//---------------------------------------------------------------
void correrSoluciones(){
	if(MazeOn){
		int casillas=0;
		restaurMazeMat();
		RunSol=1;
		readyRA=0;
		readyMD=0;
		readyMI=0;
		readyAP=0;
		readyAT=0;
		readyAF=0;
		setij();
		while(!SolReady){
			if(solRA){
				if(!readyRA){
					readyRA=random_mouse(2*BaseF+1,2*BaseC+1,&ArPant);
					gchar *display;
					display = g_strdup_printf("%d", casillas);
					gtk_label_set_text (ral, display);
					g_free(display);   
					gtk_widget_queue_draw(GTK_WIDGET(ral));
				}
					
			}
			if(solMD){
				if(!readyMD){
					readyMD=right_hand(2*BaseF+1,2*BaseC+1,&ArPant);
					gchar *display;
					display = g_strdup_printf("%d", casillas);
					gtk_label_set_text (mdl, display);
					g_free(display);   
					gtk_widget_queue_draw(GTK_WIDGET(mdl));
				}
					
			}
			if(solMI){
				if(!readyMI){
					readyMI=left_hand(2*BaseF+1,2*BaseC+1,&ArPant);
					gchar *display;
					display = g_strdup_printf("%d", casillas);
					gtk_label_set_text (mil, display);
					g_free(display);   
					gtk_widget_queue_draw(GTK_WIDGET(mil));
				}
					
			}
			if(solAP){
				if(!readyAP){
					readyAP=pledge_alg(2*BaseF+1,2*BaseC+1,&ArPant);
					gchar *display;
					display = g_strdup_printf("%d", casillas);
					gtk_label_set_text (apl, display);
					g_free(display);   
					gtk_widget_queue_draw(GTK_WIDGET(apl));
				}
					
			}
			if(solAT){
				if(!readyAT){
					readyAT=tremaux_alg(2*BaseF+1,2*BaseC+1,&ArPant);
					gchar *display;
					display = g_strdup_printf("%d", casillas);
					gtk_label_set_text (atl, display);
					g_free(display);   
					gtk_widget_queue_draw(GTK_WIDGET(atl));
				}
					
			}
			/*if(solAF){
				if(!readyAF){
					readyAF=FATTHA(2*BaseF+1,2*BaseC+1,&ArPant);
					gchar *display;
					display = g_strdup_printf("%d", casillas);
					gtk_label_set_text (afl, display);
					g_free(display);  
					gtk_widget_queue_draw(GTK_WIDGET(afl)); 
				}
			}*/
			
			if(inScreen(raton.i,raton.j)||inScreen(mano_der.i,mano_der.j)||inScreen(mano_izq.i,mano_izq.j)||inScreen(pledge.i,pledge.j)||inScreen(tremaux.i,tremaux.j)){
				do_drawing();
				gtk_widget_queue_draw(GTK_WIDGET(DrawArea));
			}
			
			casillas++;
			g_timeout_add(15,NULL,NULL);
			while(!RunSol){
				gtk_main_iteration ();
			}
		}
		SolReady=0;
	}
}

//------------------------------------------------------------------
int abrir(){
    if (abrir_maze(archivo))
        return 1;
    return 0;
}

void configspnbttn(){
	adjustmentf = gtk_adjustment_new (0, 2, 2048, 1, 0, 0);
	adjustmentc = gtk_adjustment_new (0, 2, 2048, 1, 0, 0);
	gtk_spin_button_set_adjustment (filas,adjustmentf);
	gtk_spin_button_set_adjustment (columnas,adjustmentc);
	gtk_widget_add_events(GTK_WIDGET(DrawArea), GDK_SCROLL_MASK);
	gtk_widget_add_events(GTK_WIDGET(DrawArea), GDK_BUTTON_PRESS_MASK);
	gtk_widget_add_events(GTK_WIDGET(DrawArea), GDK_POINTER_MOTION_MASK);
	g_signal_connect(G_OBJECT(DrawArea), "draw",G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(G_OBJECT(DAdial), "key_press_event", G_CALLBACK(check_escape), NULL);
    g_signal_connect(G_OBJECT(DAdial), "delete-event", G_CALLBACK(on_Maze_area_delete_event), NULL);
    g_signal_connect (G_OBJECT(DrawArea),"configure-event",G_CALLBACK (configure_event_cb), NULL);
    g_signal_connect(G_OBJECT(DrawArea), "scroll-event", G_CALLBACK(scrollZ), NULL);
    g_signal_connect(G_OBJECT(DrawArea), "motion-notify-event", G_CALLBACK(mover), NULL);
    g_signal_connect (G_OBJECT(DrawArea), "button-press-event", G_CALLBACK (button_press_event_cb), NULL);
    g_signal_connect (G_OBJECT(switchB), "notify::active", G_CALLBACK (activate_cb), window);
	}

