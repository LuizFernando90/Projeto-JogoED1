#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define LARGURA 800
#define ALTURA 600
#define MAX_MOEDAS 5

// ENUM (estado do jogador)
typedef enum
{
    PARADO,
    ANDANDO
} Estado;

// STRUCT ANINHADA
typedef struct
{
    int x;
    int y;
} Posicao;

// STRUCT PRINCIPAL
typedef struct
{
    char nome[50];
    int pontuacao;
    Estado estado;
    Posicao pos;
} Jogador;

// STRUCT MOEDA
typedef struct
{
    Posicao pos;
    int valor;
    int ativa;
} Moeda;

// FUNÇÃO para mover jogador (ponteiro)
void mover_jogador(Jogador *j, int dx, int dy)
{
    j->pos.x += dx;
    j->pos.y += dy;

    if (dx != 0 || dy != 0)
        j->estado = ANDANDO;
    else
        j->estado = PARADO;
}

// FUNÇÃO para colisão
int verificar_colisao(Jogador *j, Moeda *m)
{
    if (m->ativa &&
        j->pos.x == m->pos.x &&
        j->pos.y == m->pos.y)
    {
        return 1;
    }
    return 0;
}

// FUNÇÃO para coletar moeda
void coletar_moeda(Jogador *j, Moeda *m)
{
    j->pontuacao += m->valor;
    m->ativa = 0;
}

int main()
{

    // INICIALIZA ALLEGRO
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    ALLEGRO_DISPLAY *display = al_create_display(LARGURA, ALTURA);
    ALLEGRO_EVENT_QUEUE *fila = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60);

    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_register_event_source(fila, al_get_display_event_source(display));

    // JOGADOR
    Jogador jogador;
    strcpy(jogador.nome, "Heroi"); // STRING COPY
    jogador.pontuacao = 0;
    jogador.pos.x = 100;
    jogador.pos.y = 100;
    jogador.estado = PARADO;

    // ALOCAÇÃO DINÂMICA DE MOEDAS
    Moeda *moedas = malloc(sizeof(Moeda) * MAX_MOEDAS);

    // INICIALIZA MOEDAS (vetor de structs)
    for (int i = 0; i < MAX_MOEDAS; i++)
    {
        moedas[i].pos.x = 200 + i * 60;
        moedas[i].pos.y = 200;
        moedas[i].valor = 10;
        moedas[i].ativa = 1;
    }

    // MATRIZ (mapa simples)
    int mapa[5][10] = {0};

    int rodando = 1;
    int teclas[4] = {0};

    al_start_timer(timer);

    while (rodando)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER)
        {

            int dx = 0, dy = 0;

            if (teclas[0])
                dx -= 5;
            if (teclas[1])
                dx += 5;
            if (teclas[2])
                dy -= 5;
            if (teclas[3])
                dy += 5;

            mover_jogador(&jogador, dx, dy); // ponteiro

            // VERIFICA COLISÃO COM MOEDAS
            for (int i = 0; i < MAX_MOEDAS; i++)
            {
                if (verificar_colisao(&jogador, &moedas[i]))
                {
                    coletar_moeda(&jogador, &moedas[i]);
                }
            }

            // DESENHO
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // jogador
            al_draw_filled_rectangle(
                jogador.pos.x, jogador.pos.y,
                jogador.pos.x + 30, jogador.pos.y + 30,
                al_map_rgb(0, 255, 0));

            // moedas
            for (int i = 0; i < MAX_MOEDAS; i++)
            {
                if (moedas[i].ativa)
                {
                    al_draw_filled_circle(
                        moedas[i].pos.x,
                        moedas[i].pos.y,
                        10,
                        al_map_rgb(255, 255, 0));
                }
            }

            al_flip_display();
        }

        // TECLADO
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                rodando = 0;
            if (ev.keyboard.keycode == ALLEGRO_KEY_A)
                teclas[0] = 1;
            if (ev.keyboard.keycode == ALLEGRO_KEY_D)
                teclas[1] = 1;
            if (ev.keyboard.keycode == ALLEGRO_KEY_W)
                teclas[2] = 1;
            if (ev.keyboard.keycode == ALLEGRO_KEY_S)
                teclas[3] = 1;
        }

        if (ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            if (ev.keyboard.keycode == ALLEGRO_KEY_A)
                teclas[0] = 0;
            if (ev.keyboard.keycode == ALLEGRO_KEY_D)
                teclas[1] = 0;
            if (ev.keyboard.keycode == ALLEGRO_KEY_W)
                teclas[2] = 0;
            if (ev.keyboard.keycode == ALLEGRO_KEY_S)
                teclas[3] = 0;
        }

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            rodando = 0;
        }
    }

    // LIBERA MEMÓRIA
    free(moedas);

    al_destroy_display(display);
    al_destroy_event_queue(fila);
    al_destroy_timer(timer);

    return 0;
}