#include <iostream>
#include <fstream>

#include "json_reader.h"
#include "transport_catalogue.h"

int main() {
    /*
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массива "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON
     */
    // std::ifstream in("D:/DEV_HOME/S10T1L14/build/Debug/input_2.json");
    // std::cin.rdbuf(in.rdbuf());
    trans_cat::TransportCatalogue cat;
    json_reader::JsonReader reader;
    // reader.RequestProcessing(cat, std::cin, std::cout);
 //   reader.RequestProcessing(cat, std::cin, std::cout);
}