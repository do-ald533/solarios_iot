#include "FirebaseManager.h"
#include "config.h"

void FirebaseManager::begin() {
    _config.database_url = Config::Firebase::DATABASE_URL;
    _config.signer.tokens.legacy_token = Config::Firebase::DATABASE_SECRET;
    _config.timeout.serverResponse = Config::Firebase::SERVER_TIMEOUT_MS;
    _config.timeout.socketConnection = Config::Firebase::SOCKET_TIMEOUT_MS;

    _fbdo.setBSSLBufferSize(Config::Firebase::RX_BUFFER_SIZE, Config::Firebase::TX_BUFFER_SIZE);
    _fbdo.setResponseSize(Config::Firebase::RX_BUFFER_SIZE);

    Firebase.begin(&_config, &_auth);
    Firebase.reconnectWiFi(true);
}

bool FirebaseManager::logVisit(const VisitData& visit) {
    if (!Firebase.ready()) {
        Serial.println("[Firebase] Error: Client not ready.");
        return false;
    }

    FirebaseJson json;
    json.set("entrada", visit.timestampStart);
    json.set("saida", visit.timestampEnd);
    json.set("duracao_segundos", visit.durationSeconds);

    Serial.print("Enviando dados para o Firebase...");
    _fbdo.clear();
    if (Firebase.RTDB.pushJSON(&_fbdo, "/visitas/", &json)) {
        Serial.println(" Sucesso no Firebase!");
        return true;
    } else {
        Serial.print(" Erro ao enviar para Firebase: ");
        Serial.println(_fbdo.errorReason());
        return false;
    }
}