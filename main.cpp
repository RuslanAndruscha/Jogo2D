#include<GL/freeglut.h>
#include<vector>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#define RAND_0_1 ((float)rand() / RAND_MAX)


float const LOGICAL_WIDTH = 900.0f;
float const LOGICAL_HEIGHT = 720.0f;
float frame = 1000.0f/60.0f;
float velQueda = 10.0f; //5 posições para cada frame
int tempoSpawObjetos = 1000; //tempo em milisegundos

float tempoTremor = 0.0f;
float magnitudeTremor = 5.0f;

bool leftKeyPress = false;
bool rightKeyPress = false;

float playerSpeed = 20;

int pontuacao = 0;
int vida = 3;

struct Jogador
{
  GLfloat x, y;
  GLfloat largura, altura;
  GLfloat cor[3];
};

struct Objeto{
  GLfloat x, y;
  GLfloat lado;
  GLfloat cor[3];
};

Jogador player;
Objeto objeto;

std::vector<Objeto> vetorObjetos;

void ObjetoColisao(Objeto objeto){
  glColor3f(objeto.cor[0],objeto.cor[1],objeto.cor[2]);
  glBegin(GL_QUADS);
    glVertex2f(objeto.x,objeto.y);
    glVertex2f(objeto.x+objeto.lado,objeto.y);
    glVertex2f(objeto.x+objeto.lado,objeto.y+objeto.lado);
    glVertex2f(objeto.x,objeto.y+objeto.lado);
  glEnd();
}

void NovoObjetoColisao(){
  Objeto objeto;
  objeto.lado = 25;
  objeto.x = RAND_0_1 * (LOGICAL_WIDTH - objeto.lado);
  objeto.y = LOGICAL_HEIGHT;
  objeto.cor[0] = 1.0f;
  objeto.cor[1] = 0.0f;
  objeto.cor[2] = 0.0f;
  vetorObjetos.push_back(objeto);
}

void Player(){
  glColor3f(player.cor[0],player.cor[1],player.cor[2]);
  glBegin(GL_QUADS);
    glVertex2f(player.x,player.y);
    glVertex2f(player.x+player.largura,player.y);
    glVertex2f(player.x+player.largura,player.y+player.altura);
    glVertex2f(player.x,player.y+player.altura);
  glEnd();
}

void PlayerMovimentacao(){
  if(leftKeyPress && !rightKeyPress){
    player.x -= playerSpeed;
  }else if(rightKeyPress && !leftKeyPress){
    player.x += playerSpeed;
  }

  if(player.x <= 0){
    player.x = 0;
  }

  if(player.x + player.largura >= LOGICAL_WIDTH){
    player.x = LOGICAL_WIDTH - player.largura;
  }
}

bool ChecaColisao(Jogador p, Objeto o) {
  return((p.x < o.x + o.lado && p.x + p.largura > o.x) && (p.y < o.y + o.lado && p.y + p.altura > o.y));
}

void Inicializar(){
  srand(time(NULL));
  pontuacao = 0;
  vida = 3;
  playerSpeed = 20.0f;
  velQueda = 5.0f;
  glClearColor(1,1,1,1);
  player.x = player.x = (LOGICAL_WIDTH / 2) - 50;
  player.y = 10;
  player.altura = 25;
  player.largura = 100;
  player.cor[0] = 0.0f;
  player.cor[1] = 0.0f;
  player.cor[2] = 1.0f;

  NovoObjetoColisao();
}

void Reshape(int larguraJanela, int alturaJanela){
  // Evita divisão por zero se a janela for minimizada
  if (alturaJanela == 0) alturaJanela = 1;

  // Calcula a proporção alvo (ex: 1080 / 720 = 1.5)
  float proporcaoAlvo = LOGICAL_WIDTH / LOGICAL_HEIGHT;
  float proporcaoJanela = (float)larguraJanela / (float)alturaJanela;

  int viewportWidth, viewportHeight;
  int viewportX = 0, viewportY = 0;

  // Checa se a janela está mais "larga" ou mais "alta" que a proporção original
  if (proporcaoJanela > proporcaoAlvo) {
    // Janela muito larga: Adiciona barras pretas nas laterais (Pillarbox)
    viewportHeight = alturaJanela;
    viewportWidth = (int)(alturaJanela * proporcaoAlvo);
    viewportX = (larguraJanela - viewportWidth) / 2; // Centraliza
  } else {
    // Janela muito alta: Adiciona barras pretas em cima/baixo (Letterbox)
    viewportWidth = larguraJanela;
    viewportHeight = (int)(larguraJanela / proporcaoAlvo);
    viewportY = (alturaJanela - viewportHeight) / 2; // Centraliza
  }

  // Define a área de desenho física (estica o jogo para caber aqui)
  glViewport(viewportX, viewportY, viewportWidth, viewportHeight);

  glScissor(viewportX, viewportY, viewportWidth, viewportHeight);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, LOGICAL_WIDTH, 0, LOGICAL_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
}

void DesenhaTexto(const char* texto, float x, float y, float R, float G, float B) {
  glColor3f(R, G, B);
  glRasterPos2f(x, y);
  for (int i = 0; texto[i] != '\0'; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[i]);
  }
}

void Desenha(){
  glDisable(GL_SCISSOR_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_SCISSOR_TEST);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Cor Branca
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  if (tempoTremor > 0) {
    // Calcula um valor aleatório entre -magnitude e +magnitude
    float offsetX = ((RAND_0_1 * 2.0f) - 1.0f) * magnitudeTremor;
    float offsetY = ((RAND_0_1 * 2.0f) - 1.0f) * magnitudeTremor;
    
    glTranslatef(offsetX, offsetY, 0.0f);
  }

  Player();

  for (int i = 0; i < vetorObjetos.size(); i++)
  {
    ObjetoColisao(vetorObjetos[i]);
  }

  char textoScore[50]; 
  sprintf(textoScore, "Score: %d", pontuacao);
  DesenhaTexto(textoScore, 20, LOGICAL_HEIGHT - 40, 0.13f,0.55f,0.13f);

  char textoVidas[50];
  sprintf(textoVidas, "Vidas: %d", vida);
  DesenhaTexto(textoVidas, 20, LOGICAL_HEIGHT - 80, 1.0f,0.0f,0.0f);

  glutSwapBuffers();
}

void Teclado(unsigned char key, int x, int y){
  if(key == 27){
    exit(0);
  }
}

void TeclasEspeciais(int key, int x, int y){

  switch (key)
  {
  case GLUT_KEY_LEFT:
    leftKeyPress = true;
    break;
  case GLUT_KEY_RIGHT:
    rightKeyPress = true;
    break;
  case GLUT_KEY_F11:
    glutFullScreenToggle();
    break;
  }
}

void TeclasEspeciaisUP(int key, int x , int y){
  switch (key)
  {
  case GLUT_KEY_LEFT:
    leftKeyPress = false;
    break;
  case GLUT_KEY_RIGHT:
    rightKeyPress = false;
    break;
  }
}



void Anima(int value){
  PlayerMovimentacao();

  for(int i = 0; i < vetorObjetos.size(); i++){
    vetorObjetos[i].y -= velQueda;
    if(ChecaColisao(player,vetorObjetos[i])){
      printf("objeto colidiu!\n");
      pontuacao++;
      vetorObjetos.erase(vetorObjetos.begin() + i);
      i--;
      if(pontuacao % 20 == 0 && pontuacao) {
        velQueda += 2;
        playerSpeed += 2;
      }
      continue;
    }
    if (vetorObjetos[i].y + vetorObjetos[i].lado < 0) {
      vetorObjetos.erase(vetorObjetos.begin() + i);
      i--;
      printf("Objeto passou para baixo da tela!\n");
      vida-=1;
      tempoTremor = 300.0f;
      if(vida == 0){
        Inicializar();
        vetorObjetos.clear();
        break;
      }
    }
  }

  static float acumulador = 0.0f;
  acumulador += frame;

  if(acumulador >= tempoSpawObjetos){
    NovoObjetoColisao();
    acumulador = 0;
  }

  if (tempoTremor > 0) {
    tempoTremor -= frame;
  }

  glutTimerFunc(frame, Anima, 0);
  glutPostRedisplay();
}

int main(int argc, char **argv){
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
  glutInitWindowPosition(10,10);
  glutInitWindowSize(LOGICAL_WIDTH,LOGICAL_HEIGHT);
  glutCreateWindow("Catch Game 2D");
  glutReshapeFunc(Reshape);
  glutDisplayFunc(Desenha);
  glutKeyboardFunc(Teclado);
  glutSpecialFunc(TeclasEspeciais);
  glutSpecialUpFunc(TeclasEspeciaisUP);
  glutTimerFunc(frame, Anima, 0);
  Inicializar();

  
  glutMainLoop();
  return 0;
}
