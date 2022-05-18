#include <libTableauNoir.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

 
typedef struct {double x1; double x2; double t1; double t2; double fin;} etat_t;

typedef struct {double x; double y; double x2; double y2;} coord_t;

typedef struct {coord_t p; coord_t v;} mobile_t;

typedef struct {mobile_t o; double m;} ballon_t;

typedef struct {
    mobile_t paddle1;
    mobile_t paddle2;
    ballon_t ballon;    
} modele_t;

typedef struct {
    tStylo paddle;
    tStylo ballon;
} stylos_t;

typedef struct {
 int scorej1; int scorej2;  
} c_t;

typedef struct {
    double tx;
    double ty;
    double L;
    double w;
    int g;
} var_t;

typedef struct {
    unsigned char rouge;
    unsigned char vert;
    unsigned char bleu;
} rvb_t;

typedef struct {
    coord_t position;
    rvb_t couleur;
    int diametre;
} flocon_t;

typedef struct {
    modele_t modele;
    stylos_t stylos;
    int fini;
    var_t var;
    c_t c;
  int expl;
   flocon_t flocon[15] ;
  int fin; double x1,x2,y1,y2;
} donnees_t;

typedef struct {
 Image zoomed2,shadow2;
  int scoreLIMIT;
 } shad_t;

typedef struct {
  Image im[21];
  Image terrain,zoomed;
  Image pad1, pad2,ball,score10,score20, win1, win2,shadow;
} images;

typedef struct {
  Son drum;
  Son drum2;
  Son expl;
  int one; int diff;
  int two;int scoreLIMIT; int i; float t; int o;
  Image pong1972;
  Image  made;
  Image tunix;
  Image  press; int k,m;
} effect_t;
 
typedef struct {
  Image pong1972;
  Image  made;
  Image tunix;
  Image  press;
 
} anim_t;



/********************************/
/* déclaration des fonctions    */
/********************************/

/* initialisation */
modele_t init_modele();
stylos_t init_stylos();

/* les quatre fonctions de la boucle d'animation */
donnees_t initialiser_donnees();
void effacer(donnees_t);
void afficher(images m,donnees_t,shad_t);
void  mettre_a_jour(donnees_t *,effect_t);
int fini(donnees_t,effect_t);

double norme(coord_t u);
coord_t mult(double a, coord_t u);
coord_t plus(coord_t a, coord_t b);
coord_t vect(coord_t A, coord_t B);
double dist(coord_t A, coord_t B);
void affichageScore( images m,  donnees_t, shad_t);
void chargementImage(images *);
void menuG(effect_t*,anim_t );
void control();
void rec(effect_t*, anim_t);
void ombre(images, donnees_t, shad_t);
void Playagain(donnees_t donnees, images m, int);
void liberer_images(images m, anim_t );
void charger_SI(anim_t*, effect_t*, shad_t*);
void reset(effect_t* ,donnees_t*);



int main()
{
    creerTableau();
    choisirCouleurFond(0, 0,0);
    fixerModeBufferisation(1);
    fixerTaille(1280,720);
    
    EtatSourisClavier exit = lireEtatSourisClavier();
    while(!exit.sourisBoutonDroit && fermetureDemandee()==0)
      {
	exit = lireEtatSourisClavier();
	anim_t anim; shad_t shad; effect_t e;donnees_t donnees;images m;    /* Declaration des structures*/
	charger_SI(&anim, &e, &shad);reset(&e, &donnees); 
         menuG(&e,anim);           /* afficher le menu */
        if(e.i)
          break;
        
        donnees = initialiser_donnees();                                        
        chargementImage(&m);
        donnees.expl=0; shad.scoreLIMIT= e.scoreLIMIT;
	
        while (!fini(donnees,e) &&  fermetureDemandee()==0 ) {
             mettre_a_jour(&donnees,e);
             afficher(m,donnees,shad);
             tamponner();
             effacer(donnees);
      
       }
	
        liberer_images( m,anim  );
   	exit = lireEtatSourisClavier();
	
      }
    
	    
    return EXIT_SUCCESS;
}





flocon_t creer_flocon()
{
    flocon_t f = {
                  .position = {
                               .x = rand() % 800 - 450,
                               .y = rand() % 100 },
                  .couleur = {
                              .rouge = 255,
                              .vert  = 255,
                              .bleu  = 255},
                  .diametre = rand() % 6 + 5
    };
    return f;
}



donnees_t initialiser_donnees()
{
    donnees_t donnees = { .fini=0 };
   
    donnees.var.tx=0;
    donnees.var.ty=0;
    donnees.var.L=0;
    donnees.var.w=0;
    donnees.modele = init_modele();
    donnees.stylos = init_stylos();
    donnees.c.scorej1=0; donnees.c.scorej2=0;
    int i=0;
    
    for(i=0;i<15;++i)
      { donnees.flocon[i] = creer_flocon();
      
      }
    return donnees ;
}
 

modele_t init_modele()
{
  modele_t m = {paddle1: {p: {-600, 50, -590, -50}, v: {0, 0}},
		paddle2: {p: {600, 50, 590, -50}, v: {0, 0}},
                  ballon: {o: {p: {-6, 0, -800, 0}, v: {0, 0}}, m: 1}};
    return m;
}

 
stylos_t init_stylos()
{
    stylos_t stylos;
    choisirTypeStylo(2,255,255,255); stylos.paddle    = stockerStylo();
    choisirTypeStylo(25,255,255,255); stylos.ballon  = stockerStylo();
    return stylos;
}

/* retourne 1 si le programme doit se terminer, 0 sinon */
int fini(donnees_t donnees,effect_t e)
{
   if(donnees.c.scorej1>=e.scoreLIMIT|| donnees.c.scorej2 >=e.scoreLIMIT)
     donnees.fini=1;	
    return donnees.fini;
}


void effacer(donnees_t donnees)
{
    double x=donnees.modele.paddle1.p.x, y=donnees.modele.paddle1.p.y, x2=donnees.modele.paddle1.p.x2, y2=donnees.modele.paddle1.p.y2;
    double z=donnees.modele.paddle2.p.x, t=donnees.modele.paddle2.p.y, z2=donnees.modele.paddle2.p.x2, t2=donnees.modele.paddle2.p.y2;
    double x3=donnees.modele.ballon.o.p.x, y3=donnees.modele.ballon.o.p.y;
    
    selectionnerStylo(donnees.stylos.ballon); effacerPoint(x3, y3);
    selectionnerStylo(donnees.stylos.paddle); effacerRectangle(x, y, x2, y2); effacerRectangle(z, t, z2, t2);
   
}


/* afficher les objets mouvants et les images */
void afficher(images m,donnees_t donnees, shad_t shad)
{
  
  affichageScore(m, donnees,shad);
  double x=donnees.modele.paddle1.p.x, y=donnees.modele.paddle1.p.y, x2=donnees.modele.paddle1.p.x2, y2=donnees.modele.paddle1.p.y2;
  double z=donnees.modele.paddle2.p.x, t=donnees.modele.paddle2.p.y, z2=donnees.modele.paddle2.p.x2, t2=donnees.modele.paddle2.p.y2;
  double x3=donnees.modele.ballon.o.p.x, y3=donnees.modele.ballon.o.p.y;
  selectionnerStylo(donnees.stylos.paddle); choisirCouleurPinceau(255,255,255); tracerRectangle(x, y, x2, y2); tracerRectangle(z, t, z2, t2);
  selectionnerStylo(donnees.stylos.ballon);  tracerPoint(x3, y3);

  /* ball explosion animation*/
  if(donnees.expl==1){
   int i=0;
    for(i=0; i<15;++i)
      {choisirTypeStylo(donnees.flocon[i].diametre,
			donnees.flocon[i].couleur.rouge,
			donnees.flocon[i].couleur.vert,
                     donnees.flocon[i].couleur.bleu);
	tracerPoint(donnees.flocon[i].position.x, donnees.flocon[i].position.y);}
  }
    
  if(donnees.c.scorej1>=shad.scoreLIMIT || donnees.c.scorej2>=shad.scoreLIMIT)
    Playagain(donnees, m, shad.scoreLIMIT);

  
}


void  mettre_a_jour(donnees_t * donnees,effect_t e)
{
    
    EtatSourisClavier esc = lireEtatSourisClavier();
    double dt = delta_temps();
    if(donnees->expl==0)
      {
    double L;
    double w;
    w=donnees->var.w;
    L=donnees->var.L;
    int j1,j2;
    int vit; 
    if(e.diff==1) vit =10;
    if(e.diff==2) vit =15;
    if(e.diff==3) vit =20;
    
    if ( esc.sourisBoutonDroit) {
        donnees->fini = 1;
    }

    /* moving paddle*/ 
    if (esc.touchesClavier[82] == 1 &&  donnees->modele.paddle1.p.y <340 ) {
      donnees->modele.paddle1.p.y +=15;
      donnees->modele.paddle1.p.y2 +=15;
    }

     if (esc.touchesClavier[81] == 1 && donnees->modele.paddle1.p.y2 >-340 ) {             /* paddle 1 movements*/
       donnees->modele.paddle1.p.y -=15;
       donnees->modele.paddle1.p.y2 -=15;
    }
      if (esc.touchesClavier[26] == 1 &&  donnees->modele.paddle2.p.y <340 && e.two==1 ) {
      donnees->modele.paddle2.p.y +=15;
      donnees->modele.paddle2.p.y2 +=15;
    }

     if (esc.touchesClavier[22] == 1 && donnees->modele.paddle2.p.y2 >-340 && e.two ==1) {             /* paddle 2 movements*/
       donnees->modele.paddle2.p.y -=15;
       donnees->modele.paddle2.p.y2 -=15;
    }
     
    

   if(donnees->modele.ballon.o.p.y > donnees->modele.paddle2.p.y && donnees->modele.ballon.o.p.x >0 && e.one==1)
     {
        donnees->modele.paddle2.p.y  +=vit;                                       /* paddle 2 movements by the computer */
	donnees->modele.paddle2.p.y2 +=vit; }

    if(donnees->modele.ballon.o.p.y <  donnees->modele.paddle2.p.y2 && donnees->modele.ballon.o.p.x >0 && e.one==1)
     {
        donnees->modele.paddle2.p.y  -=vit;                                       /* paddle 2 movements by the computer */
	donnees->modele.paddle2.p.y2 -=vit; }
    
        
     
     coord_t depl;
     depl.x2= donnees->modele.ballon.o.p.x2;
     depl.y2= donnees->modele.ballon.o.p.y2;
     
      /* paddle 1 condition*/
    if(donnees->modele.ballon.o.p.x < -570  && donnees->modele.ballon.o.p.y > donnees->modele.paddle1.p.y2 && donnees->modele.ballon.o.p.y < donnees->modele.paddle1.p.y)
       {
	 donnees->x1=donnees->modele.ballon.o.p.x; donnees->y1=donnees->modele.ballon.o.p.y;
         
	  depl.y2 = donnees->modele.ballon.o.p.y-(((donnees->modele.paddle1.p.y)+ (donnees->modele.paddle1.p.y2))/2);
	   
	  donnees->modele.ballon.o.p.y2=depl.y2*13;
	  donnees->var.ty=0;
	  w= donnees->modele.ballon.o.p.y;
	  donnees->var.w=w;
	  
	  /* depl.x2= sqrt(abs((400*400)-depl.y2*6));*/
	  depl.x2=-depl.x2;
	  donnees->var.tx=0;
	  donnees->modele.ballon.o.p.x2=depl.x2;
	  L=-570;
	  donnees->var.L=L;
       
	  jouerSon(e.drum,0);
	  donnees->modele.ballon.o.p.x2+=100; /* incremente la vitesse de la balle)*/
	  
	  
       }
    /* paddle 2 condition*/
    if(donnees->modele.ballon.o.p.x > 570 && donnees->modele.ballon.o.p.y > donnees->modele.paddle2.p.y2 && donnees->modele.ballon.o.p.y < donnees->modele.paddle2.p.y)
       {
         
          donnees->x1=donnees->modele.ballon.o.p.x; donnees->y1=donnees->modele.ballon.o.p.y;
          depl.y2 = depl.y2 = donnees->modele.ballon.o.p.y-(((donnees->modele.paddle2.p.y)+ (donnees->modele.paddle2.p.y2))/2);
	  donnees->modele.ballon.o.p.y2=depl.y2*13;
	  donnees->var.ty=0;
	     
	  w= donnees->modele.ballon.o.p.y;
	   donnees->var.w=w;


	   /*depl.x2= -sqrt(abs((400*400)-depl.y2*6));*/
	   depl.x2=-depl.x2;
	  donnees->var.tx=0;
	  donnees->modele.ballon.o.p.x2=depl.x2;
	  L=570;
	  donnees->var.L=L;
	  jouerSon(e.drum,0);
	  
	  donnees->modele.ballon.o.p.x2-=100;
       }
    /* upper floor*/
    if(donnees->modele.ballon.o.p.y > 340)
       {
	 donnees->x1=donnees->modele.ballon.o.p.x; donnees->y1=donnees->modele.ballon.o.p.y;
          depl.y2= -depl.y2;
	  donnees->var.ty=0;
	  donnees->modele.ballon.o.p.y2=depl.y2;
	  w=340;
	  donnees->var.w=w;
	   jouerSon(e.drum2,0);
       }
    /* down */
     if(donnees->modele.ballon.o.p.y <- 340)
       {
	 donnees->x1=donnees->modele.ballon.o.p.x; donnees->y1=donnees->modele.ballon.o.p.y;
          depl.y2= -depl.y2;
	  donnees->var.ty=0;
	  donnees->modele.ballon.o.p.y2=depl.y2;
	  w=-340;
	  donnees->var.w=w;
	  jouerSon(e.drum2,0);
       }
     /* but*/

     int k;
      if (donnees->modele.ballon.o.p.x > 620 || donnees->modele.ballon.o.p.x < -620)
       {
	 if(donnees->modele.ballon.o.p.y >donnees->modele.paddle1.p.y ||  donnees->modele.ballon.o.p.y < donnees->modele.paddle1.p.y2 || donnees->modele.ballon.o.p.y >donnees->modele.paddle2.p.y ||  donnees->modele.ballon.o.p.y < donnees->modele.paddle2.p.y2){
	 if(donnees->modele.ballon.o.p.x > 620){   ++donnees->c.scorej1; j1=donnees->c.scorej1;j2=donnees->c.scorej2;k=-1;}
         if(donnees->modele.ballon.o.p.x <- 620){ donnees->c.scorej2++;j2=donnees->c.scorej2;j1=donnees->c.scorej1;k=1;}
	 if(donnees->modele.ballon.o.p.x <- 700) {donnees->c.scorej2=0;j2=donnees->c.scorej2;}
	 
         *donnees = initialiser_donnees();
	 donnees->c.scorej1=j1;
	 donnees->c.scorej2=j2;donnees->var.g=k;
	 if(donnees->c.scorej1 !=0 || donnees->c.scorej2!=0){
	   donnees->expl=1; 
	   jouerSon(e.expl ,0);}
	 }
       }

     if(donnees->expl==0){
       double t2; double t1;
     t1=donnees->var.tx+dt;
     t2=donnees->var.ty+dt;
     donnees->var.tx=t1;
     donnees->var.ty=t2;
     donnees->modele.ballon.o.p.x= L+(depl.x2*t1);
     if(donnees->c.scorej1 ==0 &&  donnees->c.scorej2==0 && dt > 0.1) {donnees->modele.ballon.o.p.x=-6; donnees->var.tx=0;}
     donnees->modele.ballon.o.p.y= w+(depl.y2*t2);
	 
     }


     
      }

    /* explosion animation*/
    if(donnees->expl==1){
      
      int i=0;
      for(i=0; i<7;++i)
        {
	  donnees->flocon[i].position.y = donnees->flocon[i].position.y - dt *500 ;
	  donnees->flocon[i].position.x = donnees->flocon[i].position.x +((donnees->var.g)*dt *1000) ;
	}
       for(i=7; i<15;++i)
        { donnees->flocon[i].position.y = donnees->flocon[i].position.y + dt * 500 ;
	  donnees->flocon[i].position.x = donnees->flocon[i].position.x +((donnees->var.g)*dt *1000)  ;
	  }
       if(donnees->flocon[14].position.x>720 ||donnees->flocon[14].position.x<-720 )
	 donnees->expl=0;
    }
}


double norme(coord_t u)
{
  double x = u.x*u.x;
  double y = u.y*u.y;
  return sqrt(x+y);
}  

coord_t mult(double a, coord_t u)
{
  coord_t res;
  res.x=u.x*a;
  res.y=u.y*a;
  return res;
}  

coord_t plus(coord_t a, coord_t b)
{
  coord_t res;
  res.x=a.x+b.x;
  res.y=a.y+b.y;
  return res;
}

coord_t vect(coord_t A, coord_t B)
{
  return plus(mult(-1, A), B);
}

double dist(coord_t A, coord_t B)
{
  return norme(vect(A, B));
}


void affichageScore(images m,  donnees_t donnees,shad_t shad )
{
  
  effacerTableau();
  afficherImage(m.terrain,-640,360);
  afficherImage(m.pad1,donnees.modele.paddle1.p.x-9, donnees.modele.paddle1.p.y+7);
  afficherImage(m.pad2,donnees.modele.paddle2.p.x2-10, donnees.modele.paddle2.p.y+4);
 
  if(shad.scoreLIMIT==10) afficherImage(m.score10,-30, 360);
  else afficherImage(m.score20,-30,360);
  
  afficherImage(m.im[donnees.c.scorej1] ,-300,-160);
  afficherImage(m.im[donnees.c.scorej2],100,-160);
  
  if(fabs(donnees.var.tx)>0) ombre(m,donnees,shad);
}     
      
    


void chargementImage(images *m)
{

  m->im[0] = chargerImage("Images/0"); 
  m->im[1] = chargerImage("Images/1");  
  m->im[2] = chargerImage("Images/2"); 
  m->im[3] = chargerImage("Images/3");  
  m->im[4] = chargerImage("Images/4");  
  m->im[5] = chargerImage("Images/5");  
  m->im[6] = chargerImage("Images/6");  
  m->im[7] = chargerImage("Images/7");  
  m->im[8] = chargerImage("Images/8");  
  m->im[9] = chargerImage("Images/9");  
  m->im[10] = chargerImage("Images/10");
  m->im[11] = chargerImage("Images/11"); 
  m->im[12] = chargerImage("Images/12");  
  m->im[13] = chargerImage("Images/13"); 
  m->im[14] = chargerImage("Images/14");  
  m->im[15] = chargerImage("Images/15");  
  m->im[16] = chargerImage("Images/16");  
  m->im[17] = chargerImage("Images/17");  
  m->im[18] = chargerImage("Images/18");  
  m->im[19] = chargerImage("Images/19");  
  m->im[20] = chargerImage("Images/20");
  m->terrain =chargerImage("Images/terrain");
  m->pad1 =chargerImage("Images/pad1");
  m->pad2 =chargerImage("Images/pad2");
  m->score10=chargerImage("Images/score10.png");
  m->score20=chargerImage("Images/score20.png");
  m->win1=chargerImage("Images/WINNER1");
  m->win2=chargerImage("Images/WINNER");
  m->shadow=chargerImage("Images/shado.png");

}
  
void menuG(effect_t* e, anim_t anim)
{
  
   Image menu =chargerImage("Images/menu");
   Image menu2 =chargerImage("Images/menu2");
   Image menu3S =chargerImage("Images/menu3.score");
   Image menu3D =chargerImage("Images/menu3D");
   Image menu4 =chargerImage("Images/menu4");
   Image menubot =chargerImage("Images/menubot");
   Image menubot1 =chargerImage("Images/menubot1");
   Son click =chargerAudio ("Sons/click.ogg");
   Image curseur=chargerImage("Images/curseur.png");
   
   
   
   int r,v,b;
   EtatSourisClavier boutonJouer = lireEtatSourisClavier();
    rec(e,anim);
    afficherImage(menu,-640,360);
     int c=1;
    tamponner();
    while(e->i && fermetureDemandee()==0)
    {
     
      if(sourisBoutonGaucheRelache()==1 ){
       
	if(c==1)
	  {jouerSon(click,0);
	    c=0;}
        }
      
      if(boutonJouer.touchesClavier[9]==1)              /*game controls*/
	{
	  jouerSon(click,0);
	  control();
	  jouerSon(click,0);
	  effacerTableau;
	  afficherImage(menu,-640,360);
	  tamponner();
	  attendreNms(200);
	}
      boutonJouer = lireEtatSourisClavier();
      chargerPixelsEnMemoire();
      lirePixel(boutonJouer.sourisX, boutonJouer.sourisY,&r, &v,&b);
     
      
      if (boutonJouer.sourisY <100 &&  boutonJouer.sourisY> 20 &&  boutonJouer.sourisX > -140 &&  boutonJouer.sourisX < 169 && boutonJouer.sourisBoutonGauche==1 )
	{
	
	  effacerTableau;
	  tamponner();
          afficherImage(menu2,-640,360);    /* 1 PLAYER */
	  e->one=1;
	  e->two=0;
	   c=1;
	}


       if (boutonJouer.sourisY <10 &&  boutonJouer.sourisY> -71 &&  boutonJouer.sourisX > -155 &&  boutonJouer.sourisX < 208 && boutonJouer.sourisBoutonGauche==1)
	{
	 
	  effacerTableau;
	  tamponner();
          afficherImage(menubot,-640,360);    /* 2 PLAYER */
	  e->one=0;
	  e->two=1;
	  c=1;
	}
       

      if (boutonJouer.sourisY <79 &&  boutonJouer.sourisY> 0 &&  boutonJouer.sourisX > -600 &&  boutonJouer.sourisX < -169 && boutonJouer.sourisBoutonGauche==1 && b>20 )
	{
	  
	  effacerTableau;
	  tamponner();
	  if(e->two)
	  afficherImage(menubot1,-640,360);
	                                                   /* score buton */
          else afficherImage(menu3S,-640,360);
	  if(e->scoreLIMIT==20)afficherImage(curseur,-600,-70);
	   if(e->scoreLIMIT==10)afficherImage(curseur,-600,-10); 
	  c=1;
	}  

     if (boutonJouer.sourisY <82 &&  boutonJouer.sourisY> 0 &&  boutonJouer.sourisX > 200 &&  boutonJouer.sourisX < 642 && boutonJouer.sourisBoutonGauche==1 && b>20)
	{
	  
	  effacerTableau;
	  tamponner();
          afficherImage(menu3D,-640,360);
	  if(e->diff==1)afficherImage(curseur,580,0);
	  if(e->diff==2) afficherImage(curseur,580,-65);          /* difficulty button*/
	  if(e->diff==3) afficherImage(curseur,580,-125);
	  
	  c=1;
	}  




      
     if( boutonJouer.sourisBoutonGauche==1 && boutonJouer.sourisY <-150 &&  boutonJouer.sourisY >-251 &&  boutonJouer.sourisX > -158 &&  boutonJouer.sourisX < 175 && (e->one==1 || e->two==1))
	{
	  
	  e->i=0;
	  effacerTableau();
	  libererImage(menu);
	  libererImage(menu2);
	  libererImage(menu3D);
	  libererImage(menu3S);    /* fermeture du menu */
	  libererImage(menu4);
	  libererImage(menubot);
	  libererImage(menubot1);
	  libererImage(curseur);
	  c=1;
	}


     if (boutonJouer.sourisY <0 &&  boutonJouer.sourisY>-52 &&  boutonJouer.sourisX >-410 &&  boutonJouer.sourisX < -332 && boutonJouer.sourisBoutonGauche==1 && b>20 )
	{
	  e->scoreLIMIT=10;
	  effacerTableau();
          if(e->two)
	     afficherImage(menubot1,-640,360);
	                                                   /* score buton */
          else afficherImage(menu3S,-640,360);                                      /*afficher le curseur sur le  bouton score*/
	  afficherImage(curseur,-600,-10);
	  tamponner();
	  c=1;
	}

     if (boutonJouer.sourisY <-53 &&  boutonJouer.sourisY>-105 &&  boutonJouer.sourisX >-410 &&  boutonJouer.sourisX < -332 && boutonJouer.sourisBoutonGauche==1 && b>20 )
	{
	  e->scoreLIMIT=20;
	  effacerTableau();
	   if(e->two)
	     afficherImage(menubot1,-640,360);
	                                                   /* score buton */      /*afficher le curseur sur le  bouton score*/
          else afficherImage(menu3S,-640,360);
	  afficherImage(curseur,-600,-70);
	  tamponner();
	  c=1;
	}

     if (boutonJouer.sourisY <0 &&  boutonJouer.sourisY>-42 &&  boutonJouer.sourisX >303 &&  boutonJouer.sourisX <462 && boutonJouer.sourisBoutonGauche==1 && b>20 )
	{
	  e->diff=1;
	  effacerTableau();
	  afficherImage(menu3D,-640,360);                    /*afficher le curseur sur le  bouton de la difficulté*/
	  afficherImage(curseur,580,0);
	  tamponner();
	  c=1;
	}
     if (boutonJouer.sourisY <-56 &&  boutonJouer.sourisY>-104 &&  boutonJouer.sourisX >303 &&  boutonJouer.sourisX <504 && boutonJouer.sourisBoutonGauche==1 && b>20 )
	{
	  e->diff=2;
	  effacerTableau();                                 /*afficher le curseur sur le  bouton de la difficulté*/
	  afficherImage(menu3D,-640,360);
	  afficherImage(curseur,580,-65);
	  tamponner();
	  c=1;
	}
     if (boutonJouer.sourisY <-116 &&  boutonJouer.sourisY>-167 &&  boutonJouer.sourisX >303 &&  boutonJouer.sourisX <462 && boutonJouer.sourisBoutonGauche==1 && b>20 )
	{
	  e->diff=3;
	  effacerTableau();
	  afficherImage(menu3D,-640,360);
	  afficherImage(curseur,580,-125);                  /*afficher le curseur sur le  bouton de la difficulté*/
	  tamponner();
	  c=1;
	}

     if(boutonJouer.sourisBoutonDroit==1)
       break;
     
    }

    
}


/* game controls function*/ 
void control()
{
  
   EtatSourisClavier control = lireEtatSourisClavier();
   Image c=chargerImage("Images/gamecontrols1.jpg");
   effacerTableau();
  
   afficherImage(c,-640,360);
    tamponner();
   int i=1;
   while(i)
     {
       control = lireEtatSourisClavier();
       if(control.sourisY <-171 &&  control.sourisY>-244 &&  control.sourisX >-123 &&  control.sourisX <114 && control.sourisBoutonGauche==1)
	 {
	   i=0;
           effacerTableau();
	   libererImage(c);
	   
           break;
	 }

     }
   

}

void rec(effect_t* e,anim_t anim)
{
  
  while(1 && fermetureDemandee()==0){
    effacerTableau();
    if(e->k==1) afficherImage(anim.made,-640,360);
    if(e->k==2) afficherImage(anim.tunix,-640,360);
    if(e->k==3){ afficherImage(anim.press,-640,360);afficherImage(anim.pong1972,-640,360);}
    float dt= delta_temps();
    e->t+= dt; 
    choisirTypeStylo(2,0,0,0);
    fixerOpacite(e->o);choisirCouleurPinceau(0,0,0);
    if(e->k <3)tracerRectangle(-640,360,640,-360);
    else tracerRectangle(-640,360,640,0);
    tamponner();
    choisirTypeStylo(2,0,0,0);choisirCouleurPinceau(0,0,0);
    
    if(e->k>2){   if(e->t>4.5)e->o=0;
                 if(e->t>4.7)e->o=255;
                 if(e->t>4.8)e->o=0;
                 if(e->t>5)e->o=255;
                 if(e->t>5.2)e->o=0;
                 if(e->t>5.5)e->o=255;}
    
     if(e->o <10 && e->k<3)
      e->m=-1;
     if(e->o <=255 && e->o >=0 && e->k<3) e->o-=dt*200*e->m;
     if(e->t >2 && e->k==1) {e->k=2;e->o=255;e->m=1;}
     if(e->t >4.4 && e->k==2) {e->k=3;e->o=255;e->m=1;}
     if(e->t >5.6) {choisirCouleurPinceau(255,255,255);choisirTypeStylo(2,255,255,255);fixerOpacite(255);effacerTableau();break;}
  }
  
  
}


/* ball shadow*/
void ombre(images m , donnees_t donnees,  shad_t shad)
{
  int i=1;int k=1; 
  double adj =fabs(donnees.x1-donnees.modele.ballon.o.p.x);
  double opp=fabs(donnees.y1-donnees.modele.ballon.o.p.y);
  double angle=atan(opp/adj);
  if (angle==0) k=0;
   if(donnees.modele.ballon.o.p.y2>0  )
    {
      i=1;
      }
   if(donnees.modele.ballon.o.p.y2>0 && donnees.modele.ballon.o.p.x2<0  )
    {
      i=-1;
      }
   
   if(donnees.modele.ballon.o.p.y2<0) i=-1;
   if(donnees.modele.ballon.o.p.y2<0 && donnees.modele.ballon.o.p.x2<0 ) i=1;
   if( donnees.modele.ballon.o.p.x2>0) m.zoomed = rotozoomImage(m.shadow,angle*i,1,1);
   if( donnees.modele.ballon.o.p.x2<0) m.zoomed = rotozoomImage(shad.shadow2,angle*i,1,1);
   
   afficherImage(m.zoomed,donnees.modele.ballon.o.p.x-(tn_largeur(m.zoomed)/2),donnees.modele.ballon.o.p.y+(tn_hauteur(m.zoomed)/2));
  
   
}


/* revenir au menu */
void Playagain(donnees_t donnees, images m, int scoreLIMIT )
{

   EtatSourisClavier escape = lireEtatSourisClavier();
    while(!escape.touchesClavier[41])
     
      {

	 if(donnees.c.scorej1>=scoreLIMIT || donnees.c.scorej2>=scoreLIMIT)
	   {

         	effacerTableau;
	        if(donnees.c.scorej1>=10)
	           afficherImage(m.win1,-640,360);
             	else
	           afficherImage(m.win2,-640,360);
	        tamponner();
	        escape = lireEtatSourisClavier();
	   }
	 else break;
	 
      }


}

void liberer_images(images m, anim_t anim )
{

    int k=0;
    effacerTableau;
    libererImage(m.win1);libererImage(m.win2);libererImage(m.pad2);libererImage(m.pad1);libererImage(m.terrain);libererImage(m.score10);
    libererImage(m.score20);libererImage(m.shadow); libererImage(anim.pong1972);
    libererImage(anim.tunix);libererImage(anim.press);libererImage(anim.made);
    for(k=0; k<21;++k)
      libererImage(m.im[k]);
}


void charger_SI(anim_t* anim, effect_t* e, shad_t* shad)
{
  shad->shadow2=chargerImage("Images/shado1.png");
  anim->pong1972=chargerImage("Images/pong1972.png");
  anim->made=chargerImage("Images/made.jpg");
  anim->tunix=chargerImage("Images/tunix.jpg");
  anim->press=chargerImage("Images/pressF.jpg");
  e->drum=chargerAudio ("Sons/drum.ogg");
  e->drum2=chargerAudio ("Sons/drum2.ogg");
  e->expl=chargerAudio("Sons/expl.ogg");
}

/* initialisation des données*/
void reset(effect_t* e ,donnees_t* donnees)
{
  	e->one=0;
	e->two=0;
	e->i=1;
	e->o=255;
	e->k=1;
	e->m=1;
        e->scoreLIMIT=10;
	e->diff=1;
        donnees->x1=0;
	donnees->x2=0;
	donnees->y1=0;
	donnees->y2=0;
}
