#include <pigpio.h>
#include <mosquitto.h>
#include <thread>
#include <string>
#include <iostream>
#include <unistd.h>

//declaração das macro do MQTT
#define IP_BROKER "192.168.X.X"
#define PORT_BROKER 1883
#define CLIENTE_ID "PTZ_SERVO"

//declaração das macros dos tópicos
#define TOPICO_X "/carro/camera/pan/"
#define TOPICO_Y "/carro/camera/tilt/"

//declaração das macros dos pinos
#define PINO_X 2
#define PINO_Y 3


void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
void setup_ptz(int servos[], int numServos);

int angleToPulse(int angle) { 
	int pulse = 1000 + (angle * 1000) / 180;
	if (pulse < 1000) pulse = 1000;
	if (pulse > 2000) pulse = 2000;
	return pulse;
};


int main(int argc, char** argv) {
    std::cout << "inicializando módulo" << std::endl;

    // Inicializa a pigpio
    if (gpioInitialise() < 0) {
        std::cerr << "Erro ao inicializar pigpio!" << std::endl;
        return 1;
    }
    
    // Defina os GPIOs usados pelos servos
    int servos[] = {PINO_X, PINO_Y};
    int numServos = sizeof(servos) / sizeof(servos[0]);

    // Configura os pinos
    for (int = 0; i < numServos; i++) {
	    gpioSetMode(servos[i], PI_OUTPUT);
	}

    setup_ptz(); // executa self-test
    //std::thread(exec_servos); 

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
    //std::thread.join();

    gpioTerminate();
    return 0;
}

void setup_ptz(int servos[], int numServos) {
	int pulse = angleToPulse(90);
		for (int i = 0; i < numServos; i++) {
			gpioServo(servos[i], pulse);
		}
}

void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message){
    if (std::string(message->topic) == TOPICO_X) {
	std::cout << "Controlando eixo X Pan" << std:endl;
	// exec	

    } else if (std::string(message->topic) == TOPICO_Y) {
	std:cout << "Controlando eixo Y Tilt" << std:endl;


    }
}


