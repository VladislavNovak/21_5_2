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

int selectFromList(std::vector<std::string> const &list) {
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

void showFloor(Floor const &floor, bool isFullInfo = true) {
    cout << "-----------------------------" << endl;
    cout << "        Этаж id ------------ : " << floor.id << endl;
    cout << "        Тип ---------------- : " << floorNames[static_cast<int>(floor.type)] << endl;
    cout << "        Высота ------------- : " << floor.height << endl;
    cout << "        Количество комнат -- : " << floor.children.size() << endl;
    if (!floor.children.empty() && isFullInfo) {
        for (auto const &room : floor.children) showRoom(room);
        cout << "-----------------------------" << endl;
    }
}

void showBuilding(Building const &building, bool isFullInfo = true) {
    cout << "-----------------------------" << endl;
    cout << "    Здание id -------------- : " << building.id << endl;
    cout << "    Тип -------------------- : " << buildingNames[static_cast<int>(building.type)] << endl;
    cout << "    Наличие печи ----------- : " << (building.isStove ? "Есть" : "Нет") << endl;
    cout << "    Количество этажей ------ : " << building.children.size() << endl;
    if (!building.children.empty() && isFullInfo) {
        for (auto const &floor : building.children) showFloor(floor);
        cout << "-----------------------------" << endl;
    }
}

void showSector(Sector const &sector, bool isFullInfo = true) {
    cout << "-----------------------------" << endl;
    cout << "Сектор id ------------------ :" << sector.id << endl;
    cout << "Количество зданий ---------- :" << sector.children.size() << endl;
    if (!sector.children.empty() && isFullInfo) {
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
vector<int> getBaseTypeNumbers(int const &sizeOfBaseTypes) {
    vector<int> baseTypes;
    baseTypes.reserve(sizeOfBaseTypes);
    for (int i = 0; i < sizeOfBaseTypes; ++i) baseTypes.push_back(i);

    return baseTypes;
}

// Исключает из базовых типов те типы, которые ранее были выбраны
// T тип parent, N - тип дочерних элементов (перечисления)
template<class T, class N>
vector<int> getAvailableTypeNumbers(T const &parent) {
    int sizeOfBaseTypes = static_cast<int>(N::undefined);
    vector<int> baseTypes = getBaseTypeNumbers(sizeOfBaseTypes);

    vector<int> existingTypes;
    existingTypes.reserve(parent.children.size());
    for (auto const &floor : parent.children) existingTypes.emplace_back(static_cast<int>(floor.type));

    // Получаем те типы, которые пока отсутствуют в building.floors
    auto availableTypes = removeIntersections(baseTypes, existingTypes);
    // Добавляем тип undefined, т.к. он будет идти по умолчанию
    availableTypes.emplace_back(static_cast<int>(N::undefined));

    return availableTypes;
}

// Набирает возможные комнаты
vector<int> getAvailableRoomTypeNumbers(Floor const &floor, BuildingType const &buildingType) {
    // Для комнат у нас нет ограничений, поэтому - доступны все типы
    if (buildingType == BuildingType::house) {
        int sizeOfBaseTypes = static_cast<int>(RoomType::undefined);
        return getBaseTypeNumbers(sizeOfBaseTypes);
    }

    // Остальные здания могут иметь лишь помещения с типом main
    return { static_cast<int>(RoomType::main) };
}

// Набирает возможные типы этажей
vector<int> getAvailableFloorTypeNumbers(Building const &building) {
    if (building.type == BuildingType::house) {
        return getAvailableTypeNumbers<Building, FloorType>(building);
    }

    // Для остальных типов - добавляем лишь первый этаж
    return { static_cast<int>(FloorType::first) };
}

// Набирает возможные типы строений
vector<int> getAvailableBuildingTypeNumbers(Sector const &sector) {
    return getAvailableTypeNumbers<Sector, BuildingType>(sector);
}


int getIndexFromAvailableTypeList(vector<int> const &availableTypeNumbers, vector<string> const &names, string const &menuPath) {
    // Преобразовываем в список string для обработки в selectFromList
    vector<string> typeNames;
    typeNames.reserve(availableTypeNumbers.size());
    for (auto const &typeNumber : availableTypeNumbers) typeNames.push_back(names[typeNumber]);

    cout << "Возможные типы: " << endl;
    auto indexType = selectFromList(typeNames);
    cout << "-----------------------------------------------" << endl;
    printf("%s: тип установлен как: %s\n", menuPath.c_str(), names[indexType].c_str());
    return indexType;
}

void removeCommand(string const &key, vector<string> &list) {
    removeKeyFromVector<string>(key, list);
}

// Позволим пользователю выбрать из доступных типов нужный ему
RoomType getRoomType(vector<int> const &availableTypeNumbers, string const &menuPath) {
    // Добавим ещё список всех возможных названий комнат
    return static_cast<RoomType>(getIndexFromAvailableTypeList(availableTypeNumbers, roomNames, menuPath));
}

// Позволим пользователю выбрать из доступных типов нужный ему
FloorType getFloorType(vector<int> const &availableTypes, string const &menuPath) {
    // Добавим ещё список всех возможных названий этажей
    return static_cast<FloorType>(getIndexFromAvailableTypeList(availableTypes, floorNames, menuPath));
}

BuildingType getBuildingType(vector<int> const &availableTypes, string const &menuPath) {
    // Добавим ещё список всех возможных названий зданий
    return static_cast<BuildingType>(getIndexFromAvailableTypeList(availableTypes, buildingNames, menuPath));
}

// --- --- --- --- --- ---

int changeNumericProperty(int propertyValue, string const &propertyName, string const &menuPath, vector<int> const &constraints = {}) {
    cout << "-----------------------------------------------" << endl;
    printf("%s: %s (%i)?\n", menuPath.c_str(), propertyName.c_str(), propertyValue);

    return (selectFromList({ "yes", "no" }) == 0) ? getUserNumeric(constraints) : propertyValue;
}

bool changeBoolProperty(bool propertyValue, string const &propertyName, string const &menuPath) {
    cout << "-----------------------------------------------" << endl;
    printf("%s: %s (%s)?\n", menuPath.c_str(), propertyName.c_str(), (propertyValue ? "есть " : "нет "));

    return (selectFromList({ "yes", "no" }) == 0) ? !propertyValue : propertyValue;
}

// availableTypes - перечень типов, которые можно создавать
void setRoom(Room &room, vector<int> const &availableTypeNumbersForRoom, BuildingType const &buildingType) {
    string menuPath = "AREA/SECTOR/BUILDING/FLOOR/ROOM";

    if (buildingType == BuildingType::house) {
        cout << "-----------------------------------------------" << endl;
        printf("%s: изменяем тип комнаты (%s)?\n", menuPath.c_str(), roomNames[static_cast<int>(room.type)].c_str());
        if (selectFromList({ "yes", "no" }) == 0) {
            room.type = getRoomType(availableTypeNumbersForRoom, menuPath);
        }
    }
    // Для всех типов зданий кроме house устанавливаем лишь один тип комнаты: main
    else if (buildingType != BuildingType::house && room.type != RoomType::main) {
        room.type = static_cast<RoomType>(availableTypeNumbersForRoom[0]);
        cout << "-----------------------------------------------" << endl;
        printf("%s: тип установлен автоматически: %s\n", menuPath.c_str(), roomNames[availableTypeNumbersForRoom[0]].c_str());
    }

    string title = "изменяем ширину комнаты";
    room.width = changeNumericProperty(room.width, title, menuPath, { 1000, 5000 });

    title = "изменяем длину комнаты";
    room.length = changeNumericProperty(room.length, title, menuPath, { 1000, 5000 });

    cout << "-----------------------------------------------" << endl;
    cout << menuPath << ": редактирование комнаты завершено" << endl;
}

Room getNewRoom(int newId, vector<int> const &availableTypeNumbersForRoom, BuildingType const &buildingType) {
    cout << "-----------------------------------------------" << endl;
    cout << "AREA/SECTOR/BUILDING/FLOOR: создана комната" << endl;
    Room room;
    room.id = newId;
    setRoom(room, availableTypeNumbersForRoom, buildingType);

    return room;
}

// availableFloorTypes - перечень этажей, которые можно создавать
// buildingType - даёт представление о том, какие комнаты доступны
void setFloor(Floor &floor, vector<int> const &availableFloorTypes, BuildingType const &buildingType) {
    string menuPath = "AREA/SECTOR/BUILDING/FLOOR";

    if (buildingType == BuildingType::house) {
        cout << "-----------------------------------------------" << endl;
        printf("%s: изменяем тип этажа (%s)?\n", menuPath.c_str(), floorNames[static_cast<int>(floor.type)].c_str());
        if (selectFromList({"yes", "no"}) == 0) {
            floor.type = getFloorType(availableFloorTypes, menuPath);
        }
    }
    // Для всех типов зданий кроме house устанавливаем лишь один тип этажа: first
    else if (buildingType != BuildingType::house && floor.type != FloorType::first) {
        floor.type = FloorType::first;
        cout << "-----------------------------------------------" << endl;
        printf("%s: тип установлен автоматически: %s\n", menuPath.c_str(), floorNames[static_cast<int>(floor.type)].c_str());
    }

    string title = "изменяем высоту этажа";
    floor.height = changeNumericProperty(floor.height, title, menuPath, { 2000, 4000 });

    // --- Изменения типов и количества комнат на этаже ---
    cout << "-----------------------------------------------" << endl;
    cout << menuPath << ": вносим изменения в список комнат на этаже?" << endl;
    showFloor(floor);
    if (selectFromList({"yes", "no"}) == 0) {
        vector<string> commands = {"add", "edit", "about", "exit"};

        while (true) {
            cout << "-----------------------------------------------" << endl;
            cout << menuPath << ": операции с комнатами этажа:" << endl;

            // Пытаемся найти пункт меню. Индекс найден, если >= 0
            auto index = findKeyIndexInVector<string>("add", commands);

            int selectedCommand;

            // Если в списке дочерних ничего нет, то сразу выбираем команду add
            if (floor.children.empty()) {
                selectedCommand = index;
            }
            // В ином случае - добавляем/удаляем пункты меню и выбираем уже из них
            else {
                // Набираем меню для здания house
                if (buildingType == BuildingType::house) {
                    if (floor.children.size() < floor.maxRoomCount && index == -1) commands.emplace_back("add");
                    else if (floor.children.size() >= floor.maxRoomCount && index >= 0) removeCommand("add", commands);
                }
                // Набираем меню для зданий кроме house
                else {
                    if (floor.children.empty() && index == -1) commands.emplace_back("add");
                    else if (!floor.children.empty() && index >= 0) removeCommand("add", commands);
                }

                selectedCommand = selectFromList(commands);
            }

            // Получаем возможные типы для rooms
            auto availableTypeNumbersForRoom = getAvailableRoomTypeNumbers(floor, buildingType);

            if (commands[selectedCommand] == "add") {
                auto newId = getAvailableIndexInRooms(floor.children);
                floor.children.emplace_back(getNewRoom(newId, availableTypeNumbersForRoom, buildingType));
            }
            else if (commands[selectedCommand] == "edit") {
                if (floor.children.empty()) {
                    cout << "Пока редактировать нечего: список пуст" << endl;
                    continue;
                }

                int selectedItemForChange;

                // Для типа House можно выбрать перечень помещений
                if (buildingType == BuildingType::house) {
                    auto numberOfRooms = floor.children.size();
                    // Если секторов больше, тогда будем выбирать из перечня
                    if (numberOfRooms > 1) {
                        cout << "Введите id помещения от 0 до " << (numberOfRooms - 1) << endl;
                        selectedItemForChange = getUserNumeric({0, (int)numberOfRooms - 1});
                    }
                }
                // Для остальных типов зданий - лишь основное помещение (оно одно с индексом 0)
                else {
                    selectedItemForChange = 0;
                }

                setRoom(floor.children[selectedItemForChange], availableTypeNumbersForRoom, buildingType);
            }
            else if (commands[selectedCommand] == "about") {
                showFloor(floor);
            }
            else if (commands[selectedCommand] == "exit") {
                break;
            }
        }
    }
}

Floor getNewFloor(int newId, vector<int> const& availableFloorTypes, BuildingType const &buildingType) {
    cout << "-----------------------------------------------" << endl;
    cout << "AREA/SECTOR/BUILDING: создан этаж" << endl;
    Floor floor;
    floor.id = newId;
    setFloor(floor, availableFloorTypes, buildingType);

    return floor;
}

void setBuilding(Building &building, vector<int> const &availableBuildingTypes) {
    string menuPath = "AREA/SECTOR/BUILDING";

    cout << "-----------------------------------------------" << endl;
    printf("%s: изменяем тип этажа (%s)?\n", menuPath.c_str(), buildingNames[static_cast<int>(building.type)].c_str());
    if (selectFromList({ "yes", "no" }) == 0) {
        building.type = getBuildingType(availableBuildingTypes, menuPath);
    }

    if (building.type == BuildingType::house || building.type == BuildingType::bathHouse) {
        string title = "изменяем наличие печи";
        building.isStove = changeBoolProperty(building.isStove, title, menuPath);
    }

    // --- Изменения типов и количества этажей в здании ---
    cout << "-----------------------------------------------" << endl;
    cout << menuPath << ": вносим изменения в список этажей в здании?" << endl;
    showBuilding(building);
    if (selectFromList({"yes", "no"}) == 0) {
        vector<string> commands = {"add", "edit", "about", "exit"};

        while (true) {
            cout << "-----------------------------------------------" << endl;
            cout << menuPath << ": операции с этажами здания:" << endl;

            // Пытаемся найти пункт меню. Индекс найден, если >= 0
            auto index = findKeyIndexInVector<string>("add", commands);

            int selectedCommand;

            // Если в списке дочерних ничего нет, то сразу выбираем команду add
            if (building.children.empty()) {
                selectedCommand = index;
            }
            // В ином случае - добавляем/удаляем пункты меню и выбираем уже из них
            else {
                // Набираем меню для здания house
                if (building.type == BuildingType::house) {
                    if (building.children.size() < building.maxFloorCountForHouse && index == -1) commands.emplace_back("add");
                    else if (building.children.size() >= building.maxFloorCountForHouse && index >= 0) removeCommand("add", commands);
                }
                // Набираем меню для зданий кроме house
                else {
                    if (building.children.empty() && index == -1) commands.emplace_back("add");
                    else if (!building.children.empty() && index >= 0) removeCommand("add", commands);
                }

                selectedCommand = selectFromList(commands);
            }

            // Получаем возможные типы для floors
            vector<int> availableFloorTypes = getAvailableFloorTypeNumbers(building);

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
                int selectedItemForChange = 0;
                auto numberOfFloors = building.children.size();
                if (numberOfFloors > 1) {
                    cout << "Введите id этажа от 0 до " << (numberOfFloors - 1) << endl;
                    selectedItemForChange = getUserNumeric({0, (int)numberOfFloors - 1});
                }

                setFloor(building.children[selectedItemForChange], availableFloorTypes, building.type);
            }
            else if (commands[selectedCommand] == "about") {
                showBuilding(building);
            }
            else if (commands[selectedCommand] == "exit") {
                break;
            }
        }
    }
}

Building getNewBuilding(int newId, vector<int> const& availableBuildingTypes) {
    cout << "-----------------------------------------------" << endl;
    cout << "AREA/SECTOR: создано здание" << endl;
    Building building;
    building.id = newId;

    setBuilding(building, availableBuildingTypes);

    return building;
}

void setSector(Sector &sector) {
    // --- Изменения типов и количества зданий на участке ---
    showSector(sector);
    cout << "Хотите внести изменения в список зданий на участке?" << endl;
    if (selectFromList({"yes", "no"}) == 0) {
        vector<string> commands = {"add", "edit", "about", "exit"};

        while (true) {
            cout << "-----------------------------------------------" << endl;
            cout << "AREA -> SECTOR -> operations with sector.buildings:" << endl;

            auto selectedCommand = sector.children.empty() ?
                                   // зданий в секторе ещё нет - автоматически переходим к команде add
                                   findKeyIndexInVector<string>("add", commands) :
                                   // в остальных случаях - выбираем одну из команд
                                   selectFromList(commands);

            // Вычисляем незанятые типы для building, т.к. они должны быть оригинальными
            auto availableBuildingTypes = getAvailableBuildingTypeNumbers(sector);

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
    if (selectFromList({"yes", "no"}) == 0) {
        vector<string> commands = {"add", "edit", "about", "exit"};

        while (true) {
            cout << "-----------------------------------------------" << endl;
            cout << "AREA -> operations with sectors:" << endl;

            // Если в диалоге выше ввели yes, это уже подразумевает, что мы хотим сразу добавить один сектор:
            auto selectedCommand = area.sectors.empty() ? 0 : selectFromList(commands);

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
        auto selectedCommand = selectFromList(commands); {
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