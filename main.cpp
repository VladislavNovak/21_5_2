#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

using std::cout;
using std::endl;
using std::vector;
using std::string;

enum class RoomType { bedroom, kitchen, bathroom, restroom, playroom, living, main, undefined };
enum class FloorType { first, second, third, undefined };
enum class BuildingType { house, garage, shed, bathHouse, undefined };

vector<string> roomNames = { "bedroom", "kitchen", "bathroom", "restroom", "playroom", "living", "main", "undefined" };
vector<string> floorNames = { "first", "second", "third", "undefined" };
vector<string> buildingNames = { "house", "garage", "shed", "bathHouse", "undefined" };

struct Room {
    int id{};
    RoomType type = RoomType::undefined;
    int width = 2000;
    int length = 1000;
};
struct Floor {
    int id{};
    FloorType type = FloorType::undefined;
    int height = 2000;
    int maxRoomCount = 4;
    vector<Room> children;
};
struct Building {
    int id{};
    BuildingType type = BuildingType::undefined;
    bool isStove = false;
    int maxFloorCountForHouse = 3;
    vector<Floor> children;
};
struct Sector {
    int id{};
    vector<Building> children;
};
struct Area {
    int id{};
    vector<Sector> sectors;
};

// --- --- --- --- ---

template<typename T, typename N>
bool isIncludes(const T &range, const N &item) {
    return std::any_of(range.begin(),
                       range.end(),
                       [&item](const N &c) { return c == item; });
}

std::string getTrimmedString(std::string str, std::string const &whiteSpaces = " \r\n\t\v\f") {
    auto start = str.find_first_not_of(whiteSpaces);
    str.erase(0, start);
    auto end = str.find_last_not_of(whiteSpaces);
    str.erase(end + 1);

    return str;
}

template<typename T>
int findKeyIndexInVector(const T &key, std::vector<T> const &list) {
    const int NOT_FOUND = -1;
    auto it = std::find_if(list.cbegin(), list.cend(), [key](const T &i){ return i == key; });

    if (it != list.cend()) {
        return (int)std::distance(list.cbegin(), it);
    }

    return NOT_FOUND;
}

template<typename T>
bool removeKeyFromVector(const T &key, vector<T> &list) {
    auto foundIndex = findKeyIndexInVector(key, list);
    if (foundIndex == -1) return false;

    list.erase(list.begin() + foundIndex);

    return true;
}

template<typename N>
vector<N> removeIntersections(vector<N> const &list, vector<N> const &intersection) {
    vector<N> results = list;

    for (int i = 0; i < intersection.size(); ++i) {
        for (int j = 0; j < results.size(); ++j) {
            if (intersection[i] == results[j]) {
                results.erase(results.begin() + j);
                --j;
            }
        }
    }

    return results;
}

std::string getUserLineString() {
    while (true) {
        std::string userLineString;
        cout << "Введите: ";
        std::getline(std::cin, userLineString);

        userLineString = getTrimmedString(userLineString);
        if (userLineString.empty()) {
            std::cout << "Строка не может быть пустой. Попробуйте снова!" << std::endl;
            continue;
        }

        return userLineString;
    }
}

void outputListToStream(std::ostream &out, std::vector<std::string> const &list, const std::string &delim = ",") {
    for (int i = 0; i < list.size(); ++i) out << list[i] << (i != list.size() - 1 ? delim : "");

    out << std::endl;
}

int getUserSelectionFromList(std::vector<std::string> const &list) {
    bool isList = list.size() > 1;

    while (true) {
        cout << (isList ? "Выберите одну из опций: " : "Введите команду : ");
        outputListToStream(std::cout, list, (isList ? "|" : ""));

        auto userInput = getUserLineString();
        // return index from list, if word found
        for (int i = 0; i < list.size(); ++i) if (list[i] == userInput) return i;

        cout << "Неверно. Попробуйте снова!" << endl;
    }
}

template<typename T>
std::string getDelimitedString(T const &list, char const delim = ',') {
    std::string delimitedString;
    for (int i = 0; i < list.size(); ++i) {
        delimitedString += std::to_string(list[i]);
        if (i != list.size() - 1) delimitedString += delim;
    }

    return delimitedString;
}

template<typename T>
T getUserInput(std::string const &restrictions = "") {
    T input;
    const char* warning = "Error. Symbol %c not in restrictions: %s.\nRepeat: ";

    while (true) {
        std::cin >> input;

        // предыдущее извлечение не удалось?
        if (std::cin.fail()) {
            // возвращаем нас в "нормальный" режим работы
            std::cin.clear();
            // и удаляем неверные входные данные
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Oops, that input is invalid. Please try again.\n";
            continue;
        }

        if ((restrictions.length() && isIncludes(restrictions, input)) || restrictions.empty()) break;

        printf(warning, input, getDelimitedString(restrictions).c_str());

        // Сбрасываем коматозное состояние cin и очищаем потом ввода
        std::cin.clear();
        fflush(stdin);
    }

    // удаляем любые посторонние входные данные
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return input;
}

int getUserNumeric(std::vector<int> const &list = {}, std::vector<int> const &excludedList = {}) {

    bool isRange = (list.size() == 2) && (list[0] < list[1]);
    bool isList = !list.empty() && (list.size() != 2 || ((list.size() == 2) && (list[0] > list[1])));
    bool isExcluded = !excludedList.empty();

    while (true) {
        bool isTrouble = false;
        cout << "Введите цифры" << ": ";
        int userInput = getUserInput<int>();

        vector<string> troubles;

        if (isRange && (userInput < list[0] || userInput > list[1])) isTrouble = true;
        if (isList && !isIncludes(list, userInput)) isTrouble = true;
        if (isExcluded && isIncludes(excludedList, userInput)) isTrouble = true;

        if (isTrouble) {
            troubles.emplace_back("Попробуйте снова. Это должно быть целое число");
            if (isRange) troubles.emplace_back("  и в диапазоне (" + std::to_string(list[0]) + " - " + std::to_string(list[1]) + ")");
            if (isList) troubles.emplace_back("  и в списке из (" + getDelimitedString(list) + ")");
            if (isExcluded) troubles.emplace_back("  и не входить в список из (" + getDelimitedString(excludedList) + ")");

            for (auto const &trouble : troubles) cout << trouble << endl;

            continue;
        }

        return userInput;
    }
}

// Получить первый пропущенный индекс в массиве. Либо новый (т.е. последний + 1)
int getAvailableIndexInRange(vector<int> const &range) {
    vector<int> tempRange = range;
    std::sort(tempRange.begin(), tempRange.end());

    int current = 0;
    while(current < tempRange.size()) {
        if (current != tempRange[current]) break;
        ++current;
    }

    return current;
}

// T -> struct of Room|Floor|Building|Sector
template <class T>
int getAvailableIndexInChildren(vector<T> const &children) {
    vector<int> range;
    range.reserve(children.size());
    for (auto const &child : children) range.push_back(child.id);

    return getAvailableIndexInRange(range);
}

int getAvailableIndexInRooms(vector<Room> const &rooms) {
    return getAvailableIndexInChildren<Room>(rooms);
}

int getAvailableIndexInFloors(vector<Floor> const &floors) {
    return getAvailableIndexInChildren<Floor>(floors);
}

int getAvailableIndexInBuildings(vector<Building> const &buildings) {
    return getAvailableIndexInChildren<Building>(buildings);
}

int getAvailableIndexInSectors(vector<Sector> const &sectors) {
    return getAvailableIndexInChildren<Sector>(sectors);
}

void showRoom(Room const &room) {
    cout << "-----------------------------" << endl;
    cout << "            Комната id ----- : " << room.id << endl;
    cout << "            Тип ------------ : " << roomNames[static_cast<int>(room.type)] << endl;
    cout << "            Ширина --------- : " << room.width << endl;
    cout << "            Длина ---------- : " << room.length << endl;
}

void showFloor(Floor const &floor) {
    cout << "-----------------------------" << endl;
    cout << "        Этаж id ------------ : " << floor.id << endl;
    cout << "        Тип ---------------- : " << floorNames[static_cast<int>(floor.type)] << endl;
    cout << "        Высота ------------- : " << floor.height << endl;
    cout << "        Количество комнат -- : " << floor.children.size() << endl;
    if (!floor.children.empty()) {
        for (auto const &room : floor.children) showRoom(room);
        cout << "-----------------------------" << endl;
    }
}

void showBuilding(Building const &building) {
    cout << "-----------------------------" << endl;
    cout << "    Здание id -------------- : " << building.id << endl;
    cout << "    Тип -------------------- : " << buildingNames[static_cast<int>(building.type)] << endl;
    cout << "    Наличие печи ----------- : " << (building.isStove ? "Есть" : "Нет") << endl;
    cout << "    Количество этажей ------ : " << building.children.size() << endl;
    if (!building.children.empty()) {
        for (auto const &floor : building.children) showFloor(floor);
        cout << "-----------------------------" << endl;
    }
}

void showSector(Sector const &sector) {
    cout << "-----------------------------" << endl;
    cout << "Сектор id ------------------ :" << sector.id << endl;
    cout << "Количество зданий ---------- :" << sector.children.size() << endl;
    if (!sector.children.empty()) {
        for (auto const &building : sector.children) showBuilding(building);
        cout << "-----------------------------" << endl;
    }
}

void showExistingSectors(vector<Sector> const &sectors) {
    if (!sectors.empty()) {
        for (auto const &sector : sectors) showSector(sector);
    }
}

// Нужно лишь количество элементов базового типа
vector<int> getBaseTypeNumbers(int const &baseTypesCount) {
    vector<int> baseTypes;
    baseTypes.reserve(baseTypesCount);
    for (int i = 0; i < baseTypesCount; ++i) baseTypes.push_back(i);

    return baseTypes;
}

// Исключает из базовых типов те типы, которые ранее были выбраны
// T тип parent, N - тип дочерних элементов (перечисления)
template<class T, class N>
vector<int> getAvailableTypes(T const &parent) {
    vector<int> baseTypes = getBaseTypeNumbers(static_cast<int>(N::undefined));

    vector<int> existingTypes;
    existingTypes.reserve(parent.children.size());
    for (auto const &floor : parent.children) existingTypes.emplace_back(static_cast<int>(floor.type));

    // Получаем те типы, которые пока отсутствуют в building.floors
    auto availableTypes = removeIntersections(baseTypes, existingTypes);
    // Добавляем тип undefined, т.к. он будет идти по умолчанию
    availableTypes.emplace_back(static_cast<int>(N::undefined));

    return availableTypes;
}

// Набирает возможные типы этажей
vector<int> getAvailableFloorTypes(Building const &building) {
    if (building.type == BuildingType::house) {
        return getAvailableTypes<Building, FloorType>(building);
    }

    // Для остальных типов - добавляем лишь первый этаж
    return { static_cast<int>(FloorType::first) };
}

// Набирает возможные типы строений
vector<int> getAvailableBuildingTypes(Sector const &sector) {
    return getAvailableTypes<Sector, BuildingType>(sector);
}

// --- --- --- --- --- ---

void changeSizeRoom(string const &menuPath, int &setting, string const &settingName, vector<int> const &constraints = {}) {
    cout << "-----------------------------------------------" << endl;
    printf("%s: %s (%i)?\n", menuPath.c_str(), settingName.c_str(), setting);
    if (getUserSelectionFromList({ "yes", "no" }) == 0) {
        setting = getUserNumeric(constraints);
    }
}

// availableTypes - перечень типов, которые можно создавать
void setRoom(Room &room, vector<int> const& availableRoomTypes, BuildingType const &buildingType) {
    string menuPath = "AREA/SECTOR/BUILDING/FLOOR/ROOM";
    // --- Изменение типа комнаты ---
    if (buildingType == BuildingType::house) {
        cout << "-----------------------------------------------" << endl;
        printf("%s: изменяем тип комнаты (%s)?\n", menuPath.c_str(), roomNames[static_cast<int>(room.type)].c_str());
        if (getUserSelectionFromList({ "yes", "no" }) == 0) {
            // Преобразовываем в список string для обработки в getUserSelectionFromList
            vector<string> availableTypesNames;
            availableTypesNames.reserve(availableRoomTypes.size());
            for (auto const & type : availableRoomTypes) {
                availableTypesNames.push_back(roomNames[type]);
            }

            cout << "Возможные типы комнат: " << endl;
            auto selectedRoomType = getUserSelectionFromList(availableTypesNames);
            room.type = static_cast<RoomType>(selectedRoomType);
            cout << "-----------------------------------------------" << endl;
            printf("%s: тип установлен как: %s\n", menuPath.c_str(), roomNames[static_cast<int>(room.type)].c_str());
        }
    }
    // Для всех типов зданий кроме house устанавливаем лишь один тип комнаты: main
    else if (buildingType != BuildingType::house && room.type != RoomType::main) {
        room.type = RoomType::main;
        cout << "-----------------------------------------------" << endl;
        printf("%s: тип установлен автоматически: %s\n", menuPath.c_str(), roomNames[static_cast<int>(room.type)].c_str());
    }

    // --- Изменение ширины комнаты ---
    changeSizeRoom(menuPath, room.width, "изменяем ширину", { 1000, 5000 });

    // --- Изменение длины комнаты ---
    changeSizeRoom(menuPath, room.length, "изменяем длину", { 1000, 5000 });

    cout << "-----------------------------------------------" << endl;
    cout << menuPath << ": редактирование комнаты завершено" << endl;
}

Room getNewRoom(int newId, vector<int> const& availableRoomTypes, BuildingType const &buildingType) {
    cout << "-----------------------------------------------" << endl;
    cout << "AREA/SECTOR/BUILDING/FLOOR: создана комната" << endl;
    Room room;
    room.id = newId;
    setRoom(room, availableRoomTypes, buildingType);

    return room;
}

// availableFloorTypes - перечень этажей, которые можно создавать
// buildingType - даёт представление о том, какие комнаты доступны
void setFloor(Floor &floor, vector<int> const& availableFloorTypes, BuildingType const &buildingType) {
    // --- Изменение типа этажа ---
    if (buildingType == BuildingType::house) {
        cout << "-----------------------------------------------" << endl;
        cout << "AREA -> SECTOR -> BUILDING -> FLOOR -> edit floor.type:" << endl;
        cout << "Текущий тип этажа: " << floorNames[static_cast<int>(floor.type)] << ". Меняем?" << endl;
        if (getUserSelectionFromList({"yes", "no"}) == 0) {
            // Преобразовываем в список string для обработки в getUserSelectionFromList
            vector<string> availableTypesNames;
            availableTypesNames.reserve(availableFloorTypes.size());
            for (auto const &type: availableFloorTypes) {
                availableTypesNames.push_back(floorNames[type]);
            }

            cout << "Возможные типы этажей: " << endl;
            auto selectedIndex = getUserSelectionFromList(availableTypesNames);
            floor.type = static_cast<FloorType>(selectedIndex);
        }
    }
    // Для всех типов зданий кроме house устанавливаем лишь один тип этажа: first
    else if (buildingType != BuildingType::house && floor.type != FloorType::first) {
        floor.type = FloorType::first;
        cout << "-----------------------------------------------" << endl;
        cout << "AREA -> SECTOR -> BUILDING -> FLOOR -> floor.type auto installed as ";
        cout << floorNames[static_cast<int>(floor.type)] << endl;
    }

    // --- Изменение высоты этажа ---
    cout << "-----------------------------------------------" << endl;
    cout << "AREA -> SECTOR -> BUILDING -> FLOOR -> edit floor.height:" << endl;
    cout << "Текущая высота этажа: " << std::to_string(floor.height) << ". Меняем?" << endl;
    if (getUserSelectionFromList({ "yes", "no" }) == 0) {
        floor.height = getUserNumeric({ 2000, 4000 });
    }

    // --- Изменения типов и количества комнат на этаже ---
    showFloor(floor);
    cout << "Хотите внести изменения в список комнат на этаже?" << endl;
    if (getUserSelectionFromList({"yes", "no"}) == 0) {
        vector<string> commands = {"add", "edit", "about", "exit"};

        while (true) {
            cout << "-----------------------------------------------" << endl;
            cout << "AREA -> SECTOR -> BUILDING -> FLOOR -> operations with floor.rooms:" << endl;

            // Пытаемся найти пункт меню. Индекс найден, если >= 0
            auto index = findKeyIndexInVector<string>("add", commands);

            // Набираем меню для здания house
            if (buildingType == BuildingType::house) {
                if ((floor.children.size() < floor.maxRoomCount) && index == -1)
                    commands.emplace_back("add");
                else if ((floor.children.size() >= floor.maxRoomCount) && index >= 0)
                    removeKeyFromVector<string>("add", commands);
            }
            // Набираем меню для зданий кроме house
            else {
                if (floor.children.empty() && index == -1)
                    commands.emplace_back("add");
                else if (!floor.children.empty() && index >= 0)
                    removeKeyFromVector<string>("add", commands);
            }

            auto selectedCommand = floor.children.empty() ?
                                   // комнат в здании ещё нет - автоматически переходим к команде add
                                   findKeyIndexInVector<string>("add", commands) :
                                   // в остальных случаях - выбираем одну из команд
                                   getUserSelectionFromList(commands);

            auto availableTypeNumbersForRoom = getBaseTypeNumbers(static_cast<int>(RoomType::undefined));

            if (commands[selectedCommand] == "add") {
                auto newId = getAvailableIndexInRooms(floor.children);
                // Получаем незанятые типы для rooms
                floor.children.emplace_back(getNewRoom(newId, availableTypeNumbersForRoom, buildingType));
            }
            else if (commands[selectedCommand] == "edit") {
                if (floor.children.empty()) {
                    cout << "Пока редактировать нечего: список пуст" << endl;
                    continue;
                }

                int selectUserItemForChange;

                // Для типа House можно выбрать перечень помещений
                if (buildingType == BuildingType::house) {
                    auto numberOfRooms = floor.children.size();
                    // Если секторов больше, тогда будем выбирать из перечня
                    if (numberOfRooms > 1) {
                        cout << "Введите id помещения от 0 до " << (numberOfRooms - 1) << endl;
                        selectUserItemForChange = getUserNumeric({0, (int)numberOfRooms - 1});
                    }
                }
                else {
                    // Для остальных типов зданий - лишь основное помещение
                    selectUserItemForChange = static_cast<int>(RoomType::main);
                }

                setRoom(floor.children[selectUserItemForChange], availableTypeNumbersForRoom, buildingType);
            }
            else if (commands[selectedCommand] == "about") {
                showFloor(floor);
            }
            else if (commands[selectedCommand] == "exit") {
                cout << "Выход из редактирования комнат" << endl;
                break;
            }
        }
    }
}

Floor getNewFloor(int newId, vector<int> const& availableFloorTypes, BuildingType const &buildingType) {
    cout << "-----------------------------------------------" << endl;
    cout << "AREA -> SECTOR -> BUILDING -> One FLOOR created" << endl;
    Floor floor;
    // id - is private properties. Получаем из родителя
    floor.id = newId;

    setFloor(floor, availableFloorTypes, buildingType);

    return floor;
}

void setBuilding(Building &building, vector<int> const& availableBuildingTypes) {
    // --- Изменение типа здания ---
    cout << "-----------------------------------------------" << endl;
    cout << "AREA -> SECTOR -> BUILDING -> edit building.type:" << endl;
    cout << "Текущий тип здания: " << buildingNames[static_cast<int>(building.type)] << ". Меняем?" << endl;
    if (getUserSelectionFromList({ "yes", "no" }) == 0) {
        vector<string> availableTypesNames;
        availableTypesNames.reserve(availableBuildingTypes.size());
        for (auto const & type : availableBuildingTypes) {
            availableTypesNames.push_back(buildingNames[type]);
        }

        cout << "Возможные типы зданий: " << endl;
        auto selectedIndex = getUserSelectionFromList(availableTypesNames);
        building.type = static_cast<BuildingType>(selectedIndex);
    }

    // --- Изменения типов и количества этажей в здании ---
    showBuilding(building);
    cout << "Хотите внести изменения в этажи в здании?" << endl;
    if (getUserSelectionFromList({"yes", "no"}) == 0) {
        vector<string> commands = {"add", "edit", "about", "exit"};

        while (true) {
            cout << "-----------------------------------------------" << endl;
            cout << "AREA -> SECTOR -> BUILDING -> operations with buildings.floors:" << endl;

            // Пытаемся найти пункт меню. Индекс найден, если >= 0
            auto index = findKeyIndexInVector<string>("add", commands);

            // Набираем меню для здания house
            if (building.type == BuildingType::house) {
                if ((building.children.size() < building.maxFloorCountForHouse) && index == -1)
                    commands.emplace_back("add");
                else if (((building.children.size() >= building.maxFloorCountForHouse)) && index >= 0)
                    removeKeyFromVector<string>("add", commands);
            }
            // Набираем меню для зданий кроме house
            else {
                if (building.children.empty() && index == -1)
                    commands.emplace_back("add");
                else if (!building.children.empty() && index >= 0)
                    removeKeyFromVector<string>("add", commands);
            }

            auto selectedCommand = building.children.empty() ?
                                   // этажей в здании ещё нет - автоматически переходим к команде add
                                   findKeyIndexInVector<string>("add", commands) :
                                   // в остальных случаях - выбираем одну из команд
                                   getUserSelectionFromList(commands);

            // Получаем незанятые типы для floors
            vector<int> availableFloorTypes = getAvailableFloorTypes(building);

            if (commands[selectedCommand] == "add") {
                auto newId = getAvailableIndexInFloors(building.children);
                building.children.push_back(getNewFloor(newId, availableFloorTypes, building.type));
            }
            else if (commands[selectedCommand] == "edit") {
                if (building.children.empty()) {
                    cout << "Пока редактировать нечего: список пуст" << endl;
                    continue;
                }

                // Сразу выберем первый этаж
                int selectUserItemForChange = 0;
                auto numberOfFloors = building.children.size();
                if (numberOfFloors > 1) {
                    cout << "Введите id этажа от 0 до " << (numberOfFloors - 1) << endl;
                    selectUserItemForChange = getUserNumeric({0, (int)numberOfFloors - 1});
                }

                setFloor(building.children[selectUserItemForChange], availableFloorTypes, building.type);
            }
            else if (commands[selectedCommand] == "about") {
                showBuilding(building);
            }
            else if (commands[selectedCommand] == "exit") {
                cout << "Выход из редактирования этажей" << endl;
                break;
            }
        }
    }

    // --- Изменения по наличию в здании печи ---
    cout << "-----------------------------------------------" << endl;
    cout << "AREA -> SECTOR -> BUILDING -> edit building.isStove:" << endl;
    if (building.type == BuildingType::house || building.type == BuildingType::bathHouse) {
        cout << (building.isStove ? "В здании есть печь. Меняем?" : "В здании нет печи. Меняем?") << endl;
        if (getUserSelectionFromList({ "yes", "no" }) == 0) {
            building.isStove = !building.isStove;
            cout << "Печь добавлена" << endl;
        }
    }
}

Building getNewBuilding(int newId, vector<int> const& availableBuildingTypes) {
    cout << "-----------------------------------------------" << endl;
    cout << "AREA -> SECTOR -> One BUILDING created" << endl;
    Building building;
    // id - is private properties. Получаем из родителя уникальный номер
    building.id = newId;

    setBuilding(building, availableBuildingTypes);

    return building;
}

void setSector(Sector &sector) {
    // --- Изменения типов и количества зданий на участке ---
    showSector(sector);
    cout << "Хотите внести изменения в список зданий на участке?" << endl;
    if (getUserSelectionFromList({"yes", "no"}) == 0) {
        vector<string> commands = {"add", "edit", "about", "exit"};

        while (true) {
            cout << "-----------------------------------------------" << endl;
            cout << "AREA -> SECTOR -> operations with sector.buildings:" << endl;

            auto selectedCommand = sector.children.empty() ?
                                   // зданий в секторе ещё нет - автоматически переходим к команде add
                                   findKeyIndexInVector<string>("add", commands) :
                                   // в остальных случаях - выбираем одну из команд
                                   getUserSelectionFromList(commands);

            // Вычисляем незанятые типы для building, т.к. они должны быть оригинальными
            auto availableBuildingTypes = getAvailableBuildingTypes(sector);

            if (commands[selectedCommand] == "add") {
                auto newId = getAvailableIndexInBuildings(sector.children);
                sector.children.push_back(getNewBuilding(newId, availableBuildingTypes));
            }
            else if (commands[selectedCommand] == "edit") {
                // Сразу выберем первое строение
                int selectUserItemForChange = 0;
                auto numberOfBuildings = sector.children.size();
                if (numberOfBuildings > 1) {
                    cout << "Введите id строения от 0 до " << (numberOfBuildings - 1) << endl;
                    selectUserItemForChange = getUserNumeric({0, (int)numberOfBuildings});
                }

                setBuilding(sector.children[selectUserItemForChange], availableBuildingTypes);
            }
            else if (commands[selectedCommand] == "about") {
                showSector(sector);
            }
            else if (commands[selectedCommand] == "exit") {
                cout << "Выход из редактирования зданий на участке" << endl;
                break;
            }
        }
    }
}

Sector getNewSector(int newId) {
    cout << "-----------------------------------------------" << endl;
    cout << "AREA -> One SECTOR created" << endl;
    Sector sector;
    // id - is private properties. Получаем из родителя уникальный номер
    sector.id = newId;

    setSector(sector);

    return sector;
}

void setArea(Area &area) {
    // Здесь можно размещать новые поля и условия
    // ---
    showExistingSectors(area.sectors);
    cout << (area.sectors.empty() ? "Хотите добавить?" : "Хотите внести изменения в список участков на территории?") << endl;
    if (getUserSelectionFromList({"yes", "no"}) == 0) {
        vector<string> commands = {"add", "edit", "about", "exit"};

        while (true) {
            cout << "-----------------------------------------------" << endl;
            cout << "AREA -> operations with sectors:" << endl;

            // Если в диалоге выше ввели yes, это уже подразумевает, что мы хотим сразу добавить один сектор:
            auto selectedCommand = area.sectors.empty() ? 0 : getUserSelectionFromList(commands);

            if (commands[selectedCommand] == "add") {
                auto newId = getAvailableIndexInSectors(area.sectors);
                area.sectors.push_back(getNewSector(newId));
            }
            else if (commands[selectedCommand] == "edit") {
                // Сразу выберем первый сектор
                int selectUserItemForChange = 0;
                auto numberOfSectors = area.sectors.size();
                // Если секторов больше, тогда будем выбирать из перечня
                if (numberOfSectors > 1) {
                    cout << "Введите id участка от 0 до " << (numberOfSectors - 1) << endl;
                    selectUserItemForChange = getUserNumeric({0, (int)numberOfSectors - 1});
                }

                setSector(area.sectors[selectUserItemForChange]);
            }
            else if (commands[selectedCommand] == "about") {
                showExistingSectors(area.sectors);
            }
            else if (commands[selectedCommand] == "exit") {
                cout << "Выход из редактирования участка" << endl;
                break;
            }
        }
    }
}

Area createArea(int newId) {
    cout << "-----------------------------------------------" << endl;
    cout << "START -> One AREA created" << endl;
    Area area;
    // id - is private properties. Получаем из списка выше
    area.id = newId;

    setArea(area);

    return area;
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    vector<Area> areas;

    cout << "-----------------------------------------------" << endl;
    cout << "START" << endl;
    Area firstArea = createArea(0);
    // Теоретически, территорий можно создать очень много. Но нам, в данном случае, нужна лишь одна
    areas.emplace_back(firstArea);

    vector<string> commands = {"edit", "about", "exit"};

    while (true) {
        cout << "-----------------------------------------------" << endl;
        cout << "COMMON MENU -> operations with area:" << endl;

        cout << "Для продолжения или окончательного завершения работы программы, введите одну из команд" << endl;
        auto selectedCommand = getUserSelectionFromList(commands); {
        }

        if (commands[selectedCommand] == "edit") {
            // Территория у нас одна единственная
            setArea(areas[0]);
        }
        else if (commands[selectedCommand] == "about") {
            showExistingSectors(areas[0].sectors);
        }
        else if (commands[selectedCommand] == "exit") {
            cout << "Программа закончила работу. До новых встреч" << endl;
            break;
        }
    }

    cout << "-----------------------------------------------" << endl;
    cout << "END" << endl;
}