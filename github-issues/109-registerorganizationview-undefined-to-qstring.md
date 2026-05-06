Title: Fix `Unable to assign [undefined] to QString` in `RegisterOrganizationView.qml`

Labels: bug, qml, typeerror, register-organization

Description:
`RegisterOrganizationView.qml` can emit `Unable to assign [undefined] to QString` when the error label binds to a missing `registerOrganizationViewModel.errorMessage` property.

Steps to Reproduce:
1. Open the organization registration screen.
2. Trigger an error response from `registerOrganizationViewModel`.
3. Observe the QML console output.

Actual Result:
QML reports `Unable to assign [undefined] to QString` from `RegisterOrganizationView.qml`.

Expected Result:
The view should display the error message safely without binding to an undefined value.

Likely Cause:
The error label binds directly to `registerOrganizationViewModel.errorMessage`, but the view model exposes the message through the view's local state instead.

Files Involved:
- `qml/RegisterOrganizationView.qml`

Acceptance Criteria:
- `RegisterOrganizationView.qml` no longer binds the error label to an undefined value.
- Error messages are stored in local QML state and shown reliably.
- The registration flow still clears and shows errors correctly.