#include "libSocket/server.h"
#include "libAllegro/AllegroCore.h"

Snake player[maxPlayers];
direc pack_server;
sync syncy;
corAux cory;

int eatedFoods[50];
int worldWidth = 2000;
int worldHeight = 2000;
enum directions { UP, DOWN, LEFT, RIGHT };
int *orientation;
int moveSpeed = 2;
bool started = false;
int actual_client = 0;
int count, z, idAtual, quantPlayers, scoreAux, l, alguem, l1 = 0;

int main()
{
    //Inicialização básica do Allegro, como vista no Client
    if(!coreInit())
        return -1;

    //Criação de uma janela 150px por 100px com o titulo "Server"
    if (!windowInit(150, 100, "Server"))
        return -1;

    struct msg_ret_t retorno;
    bool sair = false;

    orientation = (int *) malloc(1 * sizeof(int));

    *orientation = 0;

    syncy.numPlayers = 0;

    syncy.eFSize = 0;
    for(l = 0; l < 50; l++)
    {
        syncy.eatedFoods[l] = 0;
    }

    for(l = 0; l < maxPlayers; l++)
    {
        syncy.disc[l] = 0;
    }

    serverInit(maxPlayers);

    while(!sair)
    {
        startTimer();
        int id = acceptConnection();

        if(id != NO_CONNECTION)
        {
            printf("Alguem se Conectou com ID : %d\n", id);
            quantPlayers = id;

            orientation = (int *) realloc(orientation, (quantPlayers+1) * sizeof(int));

            orientation[quantPlayers] = 0;

            if(id==0){
                player[id].x = 500;
                player[id].y = 500;
            } else if(id==1){
                player[id].x = 1500;
                player[id].y = 500;
            } else if(id==2){
                player[id].x = 500;
                player[id].y = 1500;
            } else if(id==3){
                player[id].x = 1500;
                player[id].y = 1500;
            }  

            player[id].score = 20;
            player[id].radius = 20;

            syncy.numPlayers = quantPlayers;

            for(l = 0; l < 25; l++)
            {
                player[id].orientacao[l] = -1;
                if(l < (player[id].score / 20) + 5){
                    player[id].orientacao[count] = 0;
                }
            }

            sendMsgToClient(&id, sizeof(int), id);

            recvMsgFromClient(&cory, id, WAIT_FOR_IT);
            player[id].skin = cory.cor;          
        }
        
        if(quantPlayers==maxPlayers-1 && !started){
             broadcast(&quantPlayers, sizeof(int));
             started = true;
        } 
        
        if(started)
        {   
            bool valid = false;
            if (isValidID(actual_client))
            {
                recvMsgFromClient(pack_server, actual_client, WAIT_FOR_IT);
                valid = true;
            }

            if (++actual_client == maxPlayers)
                actual_client = 0;

            if (valid)
            {
                if(retorno.status != NO_MESSAGE)
                {   
                    id = retorno.client_id;
                    
                    if(pack_server.dead)
                    {
                        disconnectClient(id);
                        printf("O ID : %i se desconectou\n", id);
                        syncy.disc[id] = 1;
                        pack_server.dead = 0;
                    }

                    player[id].score = pack_server.scoreAux;
                    if(player[id].score%20 == 1)
                    {
                        player[id].orientacao[((player[id].score / 20) + 5) + 1] = 0; 
                    }

                    if(pack_server.scored)
                    {

                        char eated = false, k;
                        for (k = 0; k < 50; k++)
                        {
                            if (syncy.eatedFoods[k] == pack_server.ij)
                            {
                                eated = true;
                            }
                        }

                        if (!eated)
                        {
                            syncy.eatedFoods[syncy.eFSize] = pack_server.ij;
                            (syncy.eFSize)++;
                            
                            if(syncy.eFSize == 50)
                            {   
                                syncy.eFSize = 0;
                                syncy.eatedFoods[l1++] = 0;
                                if(l1 == 49)
                                {
                                    l1 = 0;
                                }
                            }
                        }

                        pack_server.scored = 0;
                    }

                    orientation[id] = pack_server.orientation;

                    for (count = (player[id].score / 20) + 5; count > 0; count--)
                        player[id].orientacao[count] = player[id].orientacao[count-1];

                    player[id].orientacao[0] = orientation[id];

                    player[id].x += cos(orientation[id] * 3.1415926 / 180.0) * moveSpeed;
                    player[id].y -= sin(orientation[id] * 3.1415926 / 180.0) * moveSpeed;

                    if (player[id].x > worldWidth)
                            player[id].x -= worldWidth;
                        else if (player[id].x < 0)
                            player[id].x += worldWidth;

                    if (player[id].y > worldHeight)
                            player[id].y -= worldHeight;
                        else if (player[id].y < 0)
                            player[id].y += worldHeight;

                    for(idAtual = 0; idAtual <= quantPlayers; idAtual++)
                    {
                        if(!syncy.disc[idAtual])
                        {
                            if(idAtual != id)
                            {
                                for (count = (player[id].score / 20) + 5; count > 0; count--)
                                    player[idAtual].orientacao[count] = player[idAtual].orientacao[count-1];

                                player[idAtual].orientacao[0] = orientation[idAtual];

                                (player[idAtual].x) += cos(orientation[idAtual] * 3.1415926 / 180.0) * moveSpeed;
                                (player[idAtual].y) -= sin(orientation[idAtual] * 3.1415926 / 180.0) * moveSpeed;

                                if (player[idAtual].x > worldWidth)
                                        player[idAtual].x -= worldWidth;
                                    else if (player[idAtual].x < 0)
                                        player[idAtual].x += worldWidth;

                                if (player[idAtual].y > worldHeight)
                                        player[idAtual].y -= worldHeight;
                                    else if (player[idAtual].y < 0)
                                        player[idAtual].y += worldHeight;
                            }
                        }
                    }

                } else {

                    for(idAtual = 0; idAtual <= quantPlayers; idAtual++)
                    {
                        if(!syncy.disc[idAtual])
                        {
                            for (count = (player[id].score / 20) + 5; count > 0; count--)
                                player[idAtual].orientacao[count] = player[idAtual].orientacao[count-1];

                            player[idAtual].orientacao[0] = orientation[idAtual];

                            (player[idAtual].x) += cos(orientation[idAtual] * 3.1415926 / 180.0) * moveSpeed;
                            (player[idAtual].y) -= sin(orientation[idAtual] * 3.1415926 / 180.0) * moveSpeed;

                            if (player[idAtual].x > worldWidth)
                                    player[idAtual].x -= worldWidth;
                                else if (player[idAtual].x < 0)
                                    player[idAtual].x += worldWidth;

                            if (player[idAtual].y > worldHeight)
                                    player[idAtual].y -= worldHeight;
                                else if (player[idAtual].y < 0)
                                    player[idAtual].y += worldHeight;
                        }
                    }
                }

                broadcast(&syncy, sizeof(sync));

                for(z = 0; z <= quantPlayers; z++)
                {
                    if(!syncy.disc[z])
                    {
                        broadcast(&player[z], sizeof(Snake));
                    }
                }
            }
        }

        al_clear_to_color(al_map_rgb(13, 17, 22));
        al_flip_display();
        
        FPSLimit();
    }

    free(orientation);

    allegroEnd();

    return 0;
}