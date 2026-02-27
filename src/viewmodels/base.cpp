#include "viewmodels/base.hpp"

namespace pawspective::viewmodels {
BaseViewModel::BaseViewModel(QObject* parent) : QObject(parent) {}

bool BaseViewModel::isBusy() const { return m_isBusy; }

void BaseViewModel::setIsBusy(bool value) {
    updateProperty(m_isBusy, value, [this]() { emit isBusyChanged(); });
}

}  // namespace pawspective::viewmodels
