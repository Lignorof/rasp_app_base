#include <pigpio.h>
#include <mosquitto.h>
#include <thread>
#include <string>
#include <iostream>

//declaração das macro do MQTT
#define IP_BROKER "192.168.X.X"
#define PORT_BROKER 1883
#define CLIENTE_ID "NOME_APP"

//declaração das macros dos tópicos
#define TOPICO_X ""
#define TOPICO_Y ""

//declaração das macros dos pinos
#define PINO_X 0
#define PINO_Y 1


void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);

int main(int argc, char** argv) {
    std::cout << "inicializando módulo" << std::endl;

    // Inicializa a pigpio
    if (gpioInitialise() < 0) {
        std::cerr << "Erro ao inicializar pigpio!" << std::endl;
        return 1;
    }

    mosquitto_lib_init();

    // Cria cliente MQTT
    mosquitto *mosq = mosquitto_new(CLIENTE_ID, true, nullptr);
    if(!mosq){
        std::cout << "Erro ao criar cliente MQTT\n";
        return 1;
    }

    // Define callback
    mosquitto_message_callback_set(mosq, on_message);

    // Conecta ao broker
    if(mosquitto_connect(mosq, IP_BROKER, PORT_BROKER, 60) != MOSQ_ERR_SUCCESS){
        std::cout << "Erro ao conectar no broker\n";
        return 1;
    }

    // Assina as tarefas
    mosquitto_subscribe(mosq, nullptr, TOPICO_X, 1);
    mosquitto_subscribe(mosq, nullptr, TOPICO_Y, 1);

    std::thread mqtt_thread([&]() {
        mosquitto_loop_forever(mosq, -1, 1);
    });

    mqtt_thread.join();
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    gpioTerminate();
    return 0;
}

void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message){
    if (std::string(message->topic) == TOPICO_X) {

    } else if (std::string(message->topic) == TOPICO_Y) {

    }
}
