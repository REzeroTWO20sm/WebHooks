#include <iostream>
#include "httplib.h"
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <fstream>
#include <string>
#include <regex>

using json = nlohmann::json;
using namespace std;

std::unordered_set<std::string> usersSet;

// g++ 1_serv.cpp -o serv --std=c++14 -I/usr/local/include -I/usr/include/nlohmann httplib.h

json parse_res(string name_json_file){
    std::ifstream file(name_json_file + ".json");
    json json = json::parse(file);
    return json;
}

json parse_req(const httplib::Request& req, httplib::Response& res){
    json request_parse = json::parse(req.body);
    return request_parse;
}

int string_to_number(const std::string& str) {
    int result;
    std::istringstream iss(str);
    iss >> result;
    return result;
}
template<typename T>
string number_to_str(T a_f){
    stringstream a_ss;
    string a_s;
    a_ss << a_f;
    a_s  = a_ss.str();
    return a_s;
}

void delete_all_buscket(json request_parse){
    string user_id = request_parse["session"]["user"]["user_id"];
    std::fstream file(user_id+".txt", std::ios::out | std::ios::trunc);
    file.close();
}

string show_all_buscket(json request_parse){
    string user_id = request_parse["session"]["user"]["user_id"];
    fstream fs(user_id+".txt", std::ios::app | std::ios_base::out | std::ios::in);
    string line;
    string buscket;

    while (getline(fs, line)){
        buscket += line+"\n";
    }

    return buscket;
}

string sum_of_goods(json request_parse){
    string user_id = request_parse["session"]["user"]["user_id"];
    std::regex pattern("[0-9]");
    string line;
    fstream fs(user_id+".txt", std::ios::app | std::ios_base::out | std::ios::in);
    string value_s;
    int value_i;
    int sum = 0;
    string sum_s;
    while (getline(fs, line))
    {
        auto words_begin = std::sregex_iterator(line.begin(), line.end(), pattern);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i){
            std::smatch match = *i;
            std::string match_str = match.str();
            value_s += match_str;
        }
        sum+=string_to_number(value_s);
        value_s = "";
    }
    sum_s=number_to_str(sum);
    return sum_s;
}   


void delete_from_buscket(json request_parse,string user_message){
    string user_id = request_parse["session"]["user"]["user_id"];
    string all_buscket = show_all_buscket(request_parse);
    string all_word = "";

    size_t start = all_buscket.find(user_message);
    size_t end = all_buscket.find("\n", start);
    
    if(start != string::npos && end != string::npos) {
        all_word = all_buscket.substr(start, end - start + 1);
        all_buscket.erase(start, end - start + 1);
    }
    delete_all_buscket(request_parse);

    std::fstream fs(user_id+".txt", std::ios::app | std::ios_base::out);
    fs << all_buscket << endl;
}

string take_second_word(string user_message){
    string response;
    size_t start = user_message.find(" ");
    for(int i=user_message.find(" ")+1;i<user_message.length();i++){
        response += user_message[i];
    }
    return response;
}
string take_first_word(string user_message){
    string response;
    size_t start = user_message.find(" ");
    for(int i=0;i<user_message.find(" ");i++){
        response += user_message[i];
    }
    return response;
}

void send_to_py(string user_message,json request_parse){
    json data;

    string name_of_product = take_first_word(user_message);
    string price = take_second_word(user_message);
    string user_id = request_parse["session"]["user"]["user_id"];
    data[user_id];
    data[name_of_product];
    data[price];
    //отправка
    httplib::Client client("http://localhost:5000");
    std::string json_str = data.dump();
    httplib::Headers headers = {
        {"Content-Type", "application/json"}
    };auto res = client.Post("/", headers, json_str, "application/json");
}

int main() {
    httplib::Server svr;
    
    svr.Post("/", [](const httplib::Request& req, httplib::Response& res) {
        std::regex pattern("^\\s*([^\\u0000-\\u007F]|\\w)+\\s\\d+\\s*$");
        std::regex pattern2("удалить\\s([^\\u0000-\\u007F]|\\w)+");

        string line;
        string all_buscket;

        json request_parse = parse_req(req,res);//то, что пишет пользователь в навыке
        json response_parse_button_ability = parse_res("ability");//кнопки умения
        json response_parse_to_main_page = parse_res("to_main_page");//текст и кнопка для вывода кнопок ability
        json response_parse_button_help = parse_res("help");

        string user_message = request_parse["request"]["command"];
        string user_id = request_parse["session"]["user"]["user_id"];
        std::fstream fs(user_id+".txt", std::ios::app | std::ios_base::out);

        if(request_parse["session"]["new"]){
            res.set_content(response_parse_button_ability.dump(), "text/json");
        }
        else{
            if (std::regex_match(user_message, pattern)) {
                response_parse_to_main_page["response"]["text"] = user_message + " Добавлен";
                fs << user_message << endl;
                send_to_py(user_message,request_parse);
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if (std::regex_match(user_message, pattern2)) {
                response_parse_to_main_page["response"]["text"] = take_second_word(user_message) + " удален";
                delete_from_buscket(request_parse,take_second_word(user_message));
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="добавление продукта в корзину"){
                response_parse_to_main_page["response"]["text"] = "пишите продукты";
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="сумма корзины"){
                response_parse_to_main_page["response"]["text"] = sum_of_goods(request_parse);
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="очистка корзины"){
                response_parse_to_main_page["response"]["text"] = "корзина очищена";
                delete_all_buscket(request_parse);
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="списка товаров в корзине"){
                all_buscket = show_all_buscket(request_parse);
                response_parse_to_main_page["response"]["text"] = all_buscket;
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="удаление продукта из корзины"){
                response_parse_to_main_page["response"]["text"] = "какой удалить";
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="вернуться к начальным кнопкам"){
                res.set_content(response_parse_button_ability.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="помощь"){
                res.set_content(response_parse_button_help.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="информация добавление продукта"){
                response_parse_to_main_page["response"]["text"] = "Добавление продуктов тип: слово+ цена+";
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="информация сумма корзины"){
                response_parse_to_main_page["response"]["text"] = "выводим сумму товаров в корзине, если корзина пуста, то ноль";
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="информация очистка корзины"){
                response_parse_to_main_page["response"]["text"] = "полностью очищяет корзину";
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="информация списка товаров"){
                response_parse_to_main_page["response"]["text"] = "выводит список всей корзины";
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else if(request_parse["request"]["command"]=="информация удаление продукта"){
                response_parse_to_main_page["response"]["text"] = "достаточно ввести первую букву товара, который нужно удалить, удалиться первое совпадение";
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            else{
                response_parse_to_main_page["response"]["text"] = "неправильная команда";
                res.set_content(response_parse_to_main_page.dump(), "text/json");
            }
            
        }
    });

    svr.listen("localhost", 8080);

    std::cout << "Server started at http://localhost:8080" << std::endl;

    return 0;
}