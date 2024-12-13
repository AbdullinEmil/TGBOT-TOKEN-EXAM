#include <iostream>
#include <tgbot/tgbot.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {

    std::string token = "";

    TgBot::Bot bot(token);


    std::map<int64_t, std::pair<std::string, int>> pollData;


    bot.getEvents().onCommand("в каком году умер Майкл Джексои и жив ли он?", [&bot, &pollData](TgBot::Message::Ptr message) {

        std::string pollQuestion = message->text;
        std::vector<std::string> options = {
            "2009",
            "1997",
            "он жив",
            "19 в. до н.э."
        };


        int correctAnswerIndex = 0;


        TgBot::PollOptions pollOptions(options, true, true);
        auto pollMessage = bot.sendMessage(message->chat->id, pollQuestion, false, "HTML", false, nullptr, pollOptions);


        pollData[message->chat->id] = std::make_pair(pollQuestion, correctAnswerIndex);
        });


    bot.getEvents().onCallbackQuery([&bot, &pollData](TgBot::CallbackQuery::Ptr query) {

        auto it = pollData.find(query->message->chat->id);
        if (it != pollData.end()) {

            int correctAnswerIndex = it->second.second;

            int selectedAnswerIndex = std::stoi(query->data);
            if (selectedAnswerIndex == correctAnswerIndex) {

                bot.sendMessage(query->message->chat->id, "Верно!", false, "HTML", false);
            }
            else {

                bot.sendMessage(query->message->chat->id, "Неверно! правильным ответом был: " + it->second.first, false, "HTML", false);
            }

            pollData.erase(it);
        }
        });
    auto sendPhotoFromURL = [&bot](int64_t chatId, std::string photoUrl) {
        try {
            cpr::Response r = cpr::Get(cpr::Url{ photoUrl });
            if (r.status_code == 200) {
                bot.sendPhoto(chatId, r.text, "https://www.ok-magazine.ru/images/cache/2022/8/30/heighten_527_crop_2400_1542_0_0_q90_1553691_5f7346165ce531424d97a625b.jpeg");
            }
            else {
                std::cerr << "ошибка загрузки фото: " << r.status_code << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "ошибка отправки фото: " << e.what() << std::endl;
        }
        };

    while (true) {
        try {
            bot.getUpdates();
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}