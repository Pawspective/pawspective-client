#include "viewmodels/animal_list_viewmodel.hpp"

#include <QDebug>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QStringView>
#include <QVariantMap>

#include "services/errors.hpp"

namespace pawspective::viewmodels::detail {

AnimalListInternalModel::AnimalListInternalModel(QObject* parent) : QAbstractListModel(parent) {}

int AnimalListInternalModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return m_items.size();
}

QVariant AnimalListInternalModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_items.size()) {
        return QVariant();
    }

    const auto& item = m_items.at(index.row());

    switch (role) {
        case AnimalIdRole:
            return item.id;
        case NameRole:
            return item.name;
        case DescriptionRole:
            return item.description;
        case AgeRole:
            return item.age;
        case AnimalTypeRole:
            return item.animalType;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> AnimalListInternalModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[AnimalIdRole] = "animalId";
    roles[NameRole] = "animalName";
    roles[DescriptionRole] = "animalDescription";
    roles[AgeRole] = "animalAge";
    roles[AnimalTypeRole] = "animalType";
    return roles;
}

void AnimalListInternalModel::update(const QList<models::AnimalDTO>& dtos) {
    clear();

    if (!dtos.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, dtos.size() - 1);
        for (const auto& dto : dtos) {
            Item item;
            item.id = dto.id;
            item.name = dto.name;
            item.description = dto.description.value_or("");
            item.age = dto.age;
            // TODO: map animal type to human-readable string
            switch (dto.breed.animalType) {
                case models::AnimalType::Dog:
                    item.animalType = "Dog";
                    break;
                case models::AnimalType::Cat:
                    item.animalType = "Cat";
                    break;
                default:
                    item.animalType = "Other";
            }
            m_items.append(item);
        }
        endInsertRows();
    }
}

void AnimalListInternalModel::clear() {
    if (!m_items.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);
        m_items.clear();
        endRemoveRows();
    }
}

}  // namespace pawspective::viewmodels::detail

namespace {

QString normalizeFilterValue(const QString& value) {
    QString normalized = value.trimmed().toLower();
    normalized.replace('-', '_');
    normalized.replace(' ', '_');
    return normalized;
}

std::optional<pawspective::models::AnimalType> parseAnimalType(const QString& value) {
    const QString normalized = normalizeFilterValue(value);
    if (normalized == "dog") {
        return pawspective::models::AnimalType::Dog;
    }
    if (normalized == "cat") {
        return pawspective::models::AnimalType::Cat;
    }
    if (normalized == "other" || normalized == "rabbit" || normalized == "bird") {
        return pawspective::models::AnimalType::Other;
    }
    return std::nullopt;
}

std::optional<pawspective::models::AnimalSize> parseAnimalSize(const QString& value) {
    const QString normalized = normalizeFilterValue(value);
    if (normalized == "small") {
        return pawspective::models::AnimalSize::Small;
    }
    if (normalized == "medium") {
        return pawspective::models::AnimalSize::Medium;
    }
    if (normalized == "large" || normalized == "xlarge" || normalized == "x_large" || normalized == "extra_large") {
        return pawspective::models::AnimalSize::Large;
    }
    return std::nullopt;
}

std::optional<pawspective::models::AnimalGender> parseAnimalGender(const QString& value) {
    const QString normalized = normalizeFilterValue(value);
    if (normalized == "male") {
        return pawspective::models::AnimalGender::Male;
    }
    if (normalized == "female") {
        return pawspective::models::AnimalGender::Female;
    }
    if (normalized == "unknown") {
        return pawspective::models::AnimalGender::Unknown;
    }
    return std::nullopt;
}

std::optional<pawspective::models::CareLevel> parseCareLevel(const QString& value) {
    const QString normalized = normalizeFilterValue(value);
    if (normalized == "easy" || normalized == "low") {
        return pawspective::models::CareLevel::Easy;
    }
    if (normalized == "moderate" || normalized == "medium") {
        return pawspective::models::CareLevel::Moderate;
    }
    if (normalized == "difficult" || normalized == "high") {
        return pawspective::models::CareLevel::Difficult;
    }
    if (normalized == "special_needs") {
        return pawspective::models::CareLevel::SpecialNeeds;
    }
    return std::nullopt;
}

std::optional<pawspective::models::AnimalColor> parseAnimalColor(const QString& value) {
    const QString normalized = normalizeFilterValue(value);
    if (normalized == "gray") {
        return pawspective::models::AnimalColor::Grey;
    }
    if (normalized == "ginger") {
        return pawspective::models::AnimalColor::Orange;
    }

    if (normalized == "black") {
        return pawspective::models::AnimalColor::Black;
    }
    if (normalized == "white") {
        return pawspective::models::AnimalColor::White;
    }
    if (normalized == "brown") {
        return pawspective::models::AnimalColor::Brown;
    }
    if (normalized == "grey") {
        return pawspective::models::AnimalColor::Grey;
    }
    if (normalized == "orange") {
        return pawspective::models::AnimalColor::Orange;
    }
    if (normalized == "cream") {
        return pawspective::models::AnimalColor::Cream;
    }
    if (normalized == "tan") {
        return pawspective::models::AnimalColor::Tan;
    }
    if (normalized == "golden") {
        return pawspective::models::AnimalColor::Golden;
    }
    if (normalized == "spotted") {
        return pawspective::models::AnimalColor::Spotted;
    }
    if (normalized == "striped") {
        return pawspective::models::AnimalColor::Striped;
    }
    if (normalized == "brindle") {
        return pawspective::models::AnimalColor::Brindle;
    }
    if (normalized == "mixed") {
        return pawspective::models::AnimalColor::Mixed;
    }
    return std::nullopt;
}

std::optional<pawspective::models::GoodWith> parseGoodWith(const QString& value) {
    const QString normalized = normalizeFilterValue(value);
    if (normalized == "dogs") {
        return pawspective::models::GoodWith::Dogs;
    }
    if (normalized == "cats") {
        return pawspective::models::GoodWith::Cats;
    }
    if (normalized == "children" || normalized == "kids") {
        return pawspective::models::GoodWith::Children;
    }
    if (normalized == "elderly") {
        return pawspective::models::GoodWith::Elderly;
    }
    return std::nullopt;
}

template <typename T>
std::optional<QVector<T>> parseEnumVector(
    const QVariantList& rawValues,
    const std::function<std::optional<T>(const QString&)>& parser
) {
    QVector<T> parsed;
    for (const auto& rawValue : rawValues) {
        const auto parsedValue = parser(rawValue.toString());
        if (parsedValue.has_value()) {
            parsed.append(parsedValue.value());
        }
    }

    if (parsed.isEmpty()) {
        return std::nullopt;
    }
    return parsed;
}

QString toDisplayLabel(const QString& rawValue) {
    QString normalized = rawValue;
    normalized.replace('_', ' ');
    normalized = normalized.trimmed().toLower();

    QStringList words = normalized.split(' ', Qt::SkipEmptyParts);
    for (QString& word : words) {
        if (!word.isEmpty()) {
            word[0] = word[0].toUpper();
        }
    }
    return words.join(' ');
}

QVariantMap toFilterOption(const QString& dtoField, const QString& dtoValue, const QString& displayName) {
    QVariantMap item;
    item.insert("dtoField", dtoField);
    item.insert("dtoValue", dtoValue);
    item.insert("displayName", displayName);
    return item;
}

template <typename T>
QVariantList toEnumFilterOptions(
    const std::optional<QVector<T>>& values,
    const QString& field,
    QString (*converter)(T)
) {
    QVariantList result;
    if (!values.has_value()) {
        return result;
    }

    for (const auto& value : values.value()) {
        const QString apiValue = converter(value).toUpper();
        result.append(toFilterOption(field, apiValue, toDisplayLabel(apiValue)));
    }
    return result;
}

QVariantList buildCityFilterOptions(
    const std::optional<QVector<int64_t>>& cityIds,
    const QHash<int64_t, QString>& cityNames
) {
    QVariantList result;
    if (!cityIds.has_value()) {
        return result;
    }
    for (const auto& cityId : cityIds.value()) {
        const QString idText = QString::number(cityId);
        const QString name = cityNames.value(cityId, idText);
        result.append(toFilterOption("cities", idText, name));
    }
    return result;
}

}  // namespace

namespace pawspective::viewmodels {

AnimalListViewModel::AnimalListViewModel(
    services::AnimalService& animalService,
    services::BreedService& breedService,
    services::OrganizationService& organizationService,
    services::CityService& cityService,
    QObject* parent
)
    : BaseViewModel(parent),
      m_listModel(new detail::AnimalListInternalModel(this)),
      m_animalService(animalService),
      m_breedService(breedService),
      m_organizationService(organizationService),
      m_cityService(cityService) {
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalsSuccess,
        this,
        &AnimalListViewModel::handleGetAnimalsSuccess
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalsFailed,
        this,
        &AnimalListViewModel::handleGetAnimalsFailed
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalsByOrganizationSuccess,
        this,
        &AnimalListViewModel::handleGetAnimalsByOrganizationSuccess
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalsByOrganizationFailed,
        this,
        &AnimalListViewModel::handleGetAnimalsByOrganizationFailed
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalFiltersSuccess,
        this,
        &AnimalListViewModel::handleGetAnimalFiltersSuccess
    );
    connect(
        &m_animalService,
        &services::AnimalService::getAnimalFiltersFailed,
        this,
        &AnimalListViewModel::handleGetAnimalFiltersFailed
    );
    connect(
        &m_breedService,
        &services::BreedService::getBreedsByTypeSuccess,
        this,
        &AnimalListViewModel::handleGetBreedsSuccess
    );
    connect(
        &m_breedService,
        &services::BreedService::getBreedsByTypeFailed,
        this,
        &AnimalListViewModel::handleGetBreedsFailed
    );
    connect(
        &m_cityService,
        &services::CityService::getCitiesSuccess,
        this,
        &AnimalListViewModel::handleGetCitiesSuccess
    );
    connect(&m_cityService, &services::CityService::getCitiesFailed, this, &AnimalListViewModel::handleGetCitiesFailed);
}

QAbstractListModel* AnimalListViewModel::listModel() { return m_listModel; }

void AnimalListViewModel::initialize() { loadAvailableFilters(); }

void AnimalListViewModel::cleanup() {
    if (auto internalModel = qobject_cast<detail::AnimalListInternalModel*>(m_listModel)) {
        qDebug() << "Cleaning up AnimalListViewModel, clearing internal model";
        internalModel->clear();
    }
}

void AnimalListViewModel::replaceAllAnimals(const QList<models::AnimalDTO>& animals) {
    if (auto internalModel = qobject_cast<detail::AnimalListInternalModel*>(m_listModel)) {
        internalModel->update(animals);
    }
}

void AnimalListViewModel::loadAnimalsForOrganization(qint64 organizationId) {
    m_currentOrganizationId = organizationId;
    if (organizationId <= 0) {
        if (auto internalModel = qobject_cast<detail::AnimalListInternalModel*>(m_listModel)) {
            internalModel->clear();
        }
        return;
    }

    updateProperty(m_isLoading, true, [this]() { emit isLoadingChanged(); });
    setIsBusy(true);
    m_animalService.getAnimalsByOrganization(organizationId);
}

void AnimalListViewModel::loadAnimalByFilters(const QVariantMap& filterData) {
    m_currentOrganizationId = 0;

    models::AnimalFilterDTO filter;
    QHash<QString, QVariantList> groupedFilters;

    // Extract filters array from filterData
    QVariantList selectedFilters = filterData.value("filters").toList();
    int ageMin = filterData.value("ageMin", 0).toInt();
    int ageMax = filterData.value("ageMax", 0).toInt();

    for (const auto& rawFilter : selectedFilters) {
        const QVariantMap filterEntry = rawFilter.toMap();
        const QString fieldName = filterEntry.value("dtoField").toString().trimmed();
        const QVariant fieldValue = filterEntry.value("dtoValue");

        if (fieldName.isEmpty() || !fieldValue.isValid()) {
            continue;
        }
        qDebug() << "Processing filter - field:" << fieldName << ", value:" << fieldValue;
        groupedFilters[fieldName].append(fieldValue);
    }

    if (groupedFilters.contains("breeds")) {
        QVector<int64_t> breedIds;
        for (const auto& rawBreedId : groupedFilters["breeds"]) {
            bool ok = false;
            const qint64 breedId = rawBreedId.toLongLong(&ok);
            if (ok && breedId > 0) {
                breedIds.append(breedId);
            }
        }
        if (!breedIds.isEmpty()) {
            filter.breeds = breedIds;
        }
    }

    if (groupedFilters.contains("cities")) {
        QVector<int64_t> cityIds;
        for (const auto& rawCityId : groupedFilters["cities"]) {
            bool ok = false;
            const qint64 cityId = rawCityId.toLongLong(&ok);
            if (ok && cityId > 0) {
                cityIds.append(cityId);
            }
        }
        if (!cityIds.isEmpty()) {
            filter.cities = cityIds;
        }
    }

    filter.animalTypes = parseEnumVector<models::AnimalType>(groupedFilters.value("animalTypes"), parseAnimalType);
    filter.sizes = parseEnumVector<models::AnimalSize>(groupedFilters.value("sizes"), parseAnimalSize);
    filter.genders = parseEnumVector<models::AnimalGender>(groupedFilters.value("genders"), parseAnimalGender);
    filter.careLevels = parseEnumVector<models::CareLevel>(groupedFilters.value("careLevels"), parseCareLevel);
    filter.colors = parseEnumVector<models::AnimalColor>(groupedFilters.value("colors"), parseAnimalColor);
    filter.goodWiths = parseEnumVector<models::GoodWith>(groupedFilters.value("goodWiths"), parseGoodWith);

    // Only add age filters if they were explicitly provided in filterData
    if (filterData.contains("ageMin") && ageMin >= 0) {
        filter.ageGte = ageMin;
        qDebug() << "Applied ageGte filter:" << ageMin;
    } else {
        qDebug() << "ageMin not set or invalid";
    }

    if (filterData.contains("ageMax") && ageMax >= 0) {
        filter.ageLte = ageMax;
        qDebug() << "Applied ageLte filter:" << ageMax;
    } else {
        qDebug() << "ageMax not set or invalid";
    }
    if (filter.ageGte.has_value() && filter.ageLte.has_value() && filter.ageGte.value() > filter.ageLte.value()) {
        const int correctedMin = filter.ageLte.value();
        const int correctedMax = filter.ageGte.value();
        filter.ageGte = correctedMin;
        filter.ageLte = correctedMax;
    }

    filter.limit = m_pageSize;
    filter.page = 1;
    m_currentFilter = filter;
    m_currentPage = 1;

    updateProperty(m_isLoading, true, [this]() { emit isLoadingChanged(); });
    setIsBusy(true);
    m_animalService.getAnimals(m_currentFilter);
}

void AnimalListViewModel::goToPage(int page) {
    if (page < 1 || page > m_totalPages || m_currentOrganizationId != 0) {
        return;
    }
    m_currentPage = page;
    m_currentFilter.page = page;
    updateProperty(m_isLoading, true, [this]() { emit isLoadingChanged(); });
    setIsBusy(true);
    m_animalService.getAnimals(m_currentFilter);
}

void AnimalListViewModel::nextPage() {
    if (m_currentPage < m_totalPages) {
        goToPage(m_currentPage + 1);
    }
}

void AnimalListViewModel::prevPage() {
    if (m_currentPage > 1) {
        goToPage(m_currentPage - 1);
    }
}

void AnimalListViewModel::loadAvailableFilters() { m_cityService.getCities(); }

void AnimalListViewModel::loadBreedsForAnimalTypes(const QVariantList& selectedTypes) {
    QSet<models::AnimalType> requestedTypes;
    for (const auto& rawEntry : selectedTypes) {
        QString typeValue;
        if (rawEntry.canConvert<QVariantMap>()) {
            const QVariantMap entry = rawEntry.toMap();
            typeValue = entry.value("dtoValue").toString();
            if (typeValue.isEmpty()) {
                typeValue = entry.value("displayName").toString();
            }
        } else {
            typeValue = rawEntry.toString();
        }

        const auto parsedType = parseAnimalType(typeValue);
        if (parsedType.has_value()) {
            requestedTypes.insert(parsedType.value());
        }
    }

    m_requestedBreedTypes = requestedTypes;

    if (m_requestedBreedTypes.isEmpty()) {
        if (!m_availableBreeds.isEmpty()) {
            m_availableBreeds.clear();
            emit availableFiltersChanged();
        }
        return;
    }

    if (!m_availableBreeds.isEmpty()) {
        m_availableBreeds.clear();
        emit availableFiltersChanged();
    }

    for (const auto type : m_requestedBreedTypes) {
        m_breedService.getBreedsByType(type);
    }
}

void AnimalListViewModel::handleGetAnimalsSuccess(const models::AnimalListDTO& result) {
    if (auto internalModel = qobject_cast<detail::AnimalListInternalModel*>(m_listModel)) {
        qDebug()
            << "Received" << result.items.size() << "animals by filters (page" << result.page << "of"
            << result.totalPages << ")";
        internalModel->update(result.items);
    }
    m_currentPage = result.page;
    m_totalPages = result.totalPages;
    m_totalCount = result.totalCount;
    m_pageSize = result.limit > 0 ? result.limit : m_pageSize;
    emit paginationChanged();
    updateProperty(m_isLoading, false, [this]() { emit isLoadingChanged(); });
    setIsBusy(false);
}

void AnimalListViewModel::handleGetAnimalsFailed(QSharedPointer<services::BaseError> error) {
    updateProperty(m_isLoading, false, [this]() { emit isLoadingChanged(); });
    setIsBusy(false);
    if (error) {
        qWarning() << "Failed to load animals by filters:" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void AnimalListViewModel::handleGetAnimalsByOrganizationSuccess(const QList<models::AnimalDTO>& animals) {
    if (auto internalModel = qobject_cast<detail::AnimalListInternalModel*>(m_listModel)) {
        qDebug() << "Received" << animals.size() << "animals for organization" << m_currentOrganizationId;
        internalModel->update(animals);
    }
    updateProperty(m_isLoading, false, [this]() { emit isLoadingChanged(); });
    setIsBusy(false);
}

void AnimalListViewModel::handleGetAnimalsByOrganizationFailed(QSharedPointer<services::BaseError> error) {
    updateProperty(m_isLoading, false, [this]() { emit isLoadingChanged(); });
    setIsBusy(false);
    if (error) {
        qWarning()
            << "Failed to load animals for organization" << m_currentOrganizationId << ":" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void AnimalListViewModel::handleGetAnimalFiltersSuccess(const models::AnimalFilterDTO& filters) {
    const QVariantList breeds = m_requestedBreedTypes.isEmpty() ? QVariantList() : m_availableBreeds;
    const QVariantList
        animalTypes = toEnumFilterOptions<models::AnimalType>(filters.animalTypes, "animalTypes", models::toApiString);
    const QVariantList sizes = toEnumFilterOptions<models::AnimalSize>(filters.sizes, "sizes", models::toApiString);
    const QVariantList
        genders = toEnumFilterOptions<models::AnimalGender>(filters.genders, "genders", models::toApiString);
    const QVariantList
        careLevels = toEnumFilterOptions<models::CareLevel>(filters.careLevels, "careLevels", models::toApiString);
    const QVariantList colors = toEnumFilterOptions<models::AnimalColor>(filters.colors, "colors", models::toApiString);
    const QVariantList
        goodWiths = toEnumFilterOptions<models::GoodWith>(filters.goodWiths, "goodWiths", models::toApiString);
    const QVariantList cities = buildCityFilterOptions(filters.cities, m_cityNames);

    const bool changed =
        m_availableBreeds != breeds || m_availableAnimalTypes != animalTypes || m_availableSizes != sizes ||
        m_availableGenders != genders || m_availableCareLevels != careLevels || m_availableColors != colors ||
        m_availableGoodWiths != goodWiths || m_availableCities != cities;

    if (!changed) {
        return;
    }

    m_availableBreeds = breeds;
    m_availableAnimalTypes = animalTypes;
    m_availableSizes = sizes;
    m_availableGenders = genders;
    m_availableCareLevels = careLevels;
    m_availableColors = colors;
    m_availableGoodWiths = goodWiths;
    m_availableCities = cities;
    emit availableFiltersChanged();
}

void AnimalListViewModel::handleGetAnimalFiltersFailed(QSharedPointer<services::BaseError> error) {
    if (error) {
        qWarning() << "Failed to load filter metadata:" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void AnimalListViewModel::handleGetBreedsSuccess(const QList<models::BreedDTO>& breeds) {
    if (m_requestedBreedTypes.isEmpty()) {
        return;
    }

    QSet<QString> existingIds;
    for (const auto& entry : m_availableBreeds) {
        const QVariantMap entryMap = entry.toMap();
        const QString idText = entryMap.value("dtoValue").toString();
        if (!idText.isEmpty()) {
            existingIds.insert(idText);
        }
    }

    bool changed = false;
    for (const auto& breed : breeds) {
        if (!m_requestedBreedTypes.contains(breed.animalType)) {
            continue;
        }

        const QString idText = QString::number(breed.id);
        if (existingIds.contains(idText)) {
            continue;
        }

        QVariantMap entry;
        entry["dtoField"] = "breeds";
        entry["dtoValue"] = idText;
        entry["displayName"] = breed.name;
        m_availableBreeds.append(entry);
        existingIds.insert(idText);
        changed = true;
    }

    if (changed) {
        emit availableFiltersChanged();
    }
}

void AnimalListViewModel::handleGetBreedsFailed(QSharedPointer<services::BaseError> error) {
    if (error) {
        qWarning() << "Failed to load breeds:" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
}

void AnimalListViewModel::handleGetCitiesSuccess(const QList<models::CityDTO>& cities) {
    m_cityNames.clear();
    for (const auto& city : cities) {
        m_cityNames[city.id] = city.name;
    }
    m_animalService.getAnimalFilters();
}

void AnimalListViewModel::handleGetCitiesFailed(QSharedPointer<services::BaseError> error) {
    if (error) {
        qWarning() << "Failed to load cities:" << error->getMessage();
        emitError(ErrorType::NetworkError, error->getMessage());
    }
    m_animalService.getAnimalFilters();
}

}  // namespace pawspective::viewmodels
