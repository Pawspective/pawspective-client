#include "viewmodels/post_card_viewmodel.hpp"

namespace pawspective::viewmodels {

PostCardViewModel::PostCardViewModel(QObject* parent) : QObject(parent) {}

PostCardViewModel::PostCardViewModel(const models::PostDTO& dto, QObject* parent) : QObject(parent) { setFromDTO(dto); }

QString PostCardViewModel::shortText() const {
    if (m_text.length() <= 150) {
        return m_text;
    }
    return m_text.left(150) + "...";
}

bool PostCardViewModel::hasLongText() const { return m_text.length() > 150; }

void PostCardViewModel::setIsExpanded(bool expanded) {
    if (m_isExpanded != expanded) {
        m_isExpanded = expanded;
        emit isExpandedChanged();
    }
}

void PostCardViewModel::toggleExpand() { setIsExpanded(!m_isExpanded); }

void PostCardViewModel::setFromDTO(const models::PostDTO& dto) {
    if (m_text != dto.text) {
        m_text = dto.text;
        emit textChanged();
    }

    if (m_createdAt != dto.createdAt) {
        m_createdAt = dto.createdAt;
        emit createdAtChanged();
    }

    m_isExpanded = false;
    emit isExpandedChanged();
}

}  // namespace pawspective::viewmodels
