# server
модуль для передачи по сети поверх SSL

## Описание
модуль построен на основе паттерна observer для каждого чиха (события)
хз насчёт производительности, мне нужна функциональность
я не помню как SSL примонстрячил сюда и как ваще эт юзать
## состав библиотеки

## Примеры
``` c++

int main(int argc, char const *argv[])
{
    //initialisation
    server::network::TCPServer Network;
    auto cryptoLayer = std::make_shared<server::securelayer::ssl>();
    auto httpParser = std::make_shared<server::httplayer::http>();

    //connect parser to chiper out
    cryptoLayer->add_event_listener(httpParser, server::securelayer::RDATA_READY);
    
    //connect chiper to network connection
    Network.add_event_listener(cryptoLayer, server::network::DATA_AVAILABLE);

    // std::this_thread::sleep_for(std::chrono::milliseconds(20000));
    
    Network.run();
    std::string a;
    std::cin>>a;
    Network.stop();
    return 0;
}

```
## status

в разработке ...
но вроде чё-то работало, как-то https запускал..
