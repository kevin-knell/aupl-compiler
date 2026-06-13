#pragma once

#define DECL_RID(m_type) struct m_type##RID_T; typedef m_type##RID_T* m_type##RID

namespace auplib {

DECL_RID(CanvasItem);
DECL_RID(Texture);

} // namespace auplib
