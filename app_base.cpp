#include <pigpio.h>
#include <mosquitto.h>
#include <thread>
#include <string>
#include <iostream>
#include <unistd.h>

//declaração das macro do MQTT
#define IP_BROKER "192.168.0.213"
#define PORT_BROKER 1883
#define CLIENTE_ID "PTZ_SERVO"

//declaração das macros dos tópicos
#define TOPICO_X "/carro/camera/pan/"
#define TOPICO_Y "/carro/camera/tilt/"

//declaração das macros dos pinos
#define PINO_X 2
#define PINO_Y 3


void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
void setup_ptz();
void pan_control(bool side);



void pan_control(bool side);


int main(int argc, char** argv) {
    std::cout << "inicializando módulo" << std::endl;

    // Inicializa a pigpio
    if (gpioInitialise() < 0) {
        std::cerr << "Erro ao inicializar pigpio!" << std::endl;
        return 1;
    }

	setup_ptz();


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
	if (std::string((char *)message->payload) == "increase") {
		std::cout << "Controlando eixo X Pan" << std::endl;
		pan_control(true);
	} else if (std::string((char *)message->payload) == "decrease") {
		pan_control(false);
		//gpioServo(servos[PINO_X], (pulse += 10))
	}
		

    } else if (std::string(message->topic) == TOPICO_Y) {
	
    }
}


int angleToPulse(int angle) { 
	int pulse = 1000 + (angle * 1000) / 180;
	if (pulse < 1000) pulse = 1000;
	if (pulse > 2000) pulse = 2000;
	return pulse;
};

void pan_control(bool side) {
	int i;
	if (side == true) {
		int pulse = angleToPulse(i += 10);
		gpioServo(PINO_X, pulse);
		std::cout << "Pan para direita" << std::endl;
		usleep(200000);
	} else if (side == false) {
		int pulse = angleToPulse(i -= 10);
		gpioServo(PINO_X, pulse);
		std::cout << "Pan para direita" << std::endl;
		usleep(200000);

	}
}

void setup_ptz() {

    // Defina os GPIOs usados pelos servos
    int servos[] = {PINO_X, PINO_Y};
    int numServos = sizeof(servos) / sizeof(servos[0]);

    // Configura os pinos
    for (int i = 0; i < numServos; i++) {
	    gpioSetMode(servos[i], PI_OUTPUT);
	}

    // Retorna ao centro:
	int pulse = angleToPulse(90);
	for (int i = 0; i < numServos; i++) {
		gpioServo(servos[i], pulse);
	}
	usleep(200000);
}
