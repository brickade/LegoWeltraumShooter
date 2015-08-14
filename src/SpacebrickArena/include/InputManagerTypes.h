#ifndef _INPUTMANAGERTYPES_H_
#define _INPUTMANAGERTYPES_H_

namespace sba
{
    namespace Input
    {
        // **************************************************************************
        // Inputs
        // **************************************************************************
        struct EGamepadDirection
        {
            enum Type
            {
                LeftThumb,
                RightThumb,
                DPad,
                Buttons,
                LAST = Buttons
            };
        };

        struct EKeyboardDirection
        {
            enum Type
            {
                ArrowKeys,
                WSAD,
                IKJL,
                Numpad,
                Mouse,
                LAST = Mouse
            };
        };

        struct EGamepadAxis
        {
            enum Type
            {
                LeftTrigger,
                RightTrigger,
                BothTrigger,
                LeftThumbX,
                LeftThumbY,
                RightThumbX,
                RightThumbY,
                DPadX,
                DPadY,
                LAST = DPadY
            };
        };

        struct EKeyboardAxis
        {
            enum Type
            {
                WS,
                AD,
                RF,
                QE,
                ArrowUD,
                ArrowLR,
                MouseX,
                MouseY,
                MouseScroll,
                LAST = MouseScroll
            };
        };

        struct EGamepadButton
        {
            enum Type
            {
                //PuRe_Input
                Pad_A, Pad_B, Pad_X, Pad_Y, DPAD_Left, DPAD_Right, DPAD_Up, DPAD_Down, Left_Shoulder, Right_Shoulder, Pad_Back, Pad_Start, Left_Thumb, Right_Thumb,
                PURE_LAST = Right_Thumb,
                //Own
                LAST = PURE_LAST
            };
        };

        struct EKeyboardButton
        {
            enum Type
            {
                //PuRe_Input
                Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
                A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
                Enter, Backspace, Space, Ctrl, Alt, Shift, Escape,
                F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
                Left, Up, Right, Down, Period, Comma, Minus,
                Num_Zero, Num_One, Num_Two, Num_Three, Num_Four, Num_Five, Num_Six, Num_Seven, Num_Eight, Num_Nine,
                Delete, Insert, PrintScreen,
                PURE_LAST = PrintScreen,
                //Own
                CtrlS,
                MouseLeft, MouseMiddle, MouseRight,
                LAST = MouseRight
            };
        };

        // **************************************************************************
        // Keys
        // **************************************************************************
        struct EDirection
        {
            enum Type
            {
                EditorCameraRotate,
                EditorMoveBrick,
                EditorChooseElement,
                Navigate,
                GameMove_1,
                GameMove_2,
                LAST = GameMove_2
            };
        };

        struct EAxis
        {
            enum Type
            {
                EditorCameraZoom,
                EditorColorSlider,
                GameShoot,
                GamePauseSwitch,
                GameThrust_1,
                GameThrust_2,
                GameThrust_3,
                GameSpin_1,
                GameSpin_2,
                LAST = GameSpin_2
            };
        };

        struct EButton
        {
            enum Type
            {
                EditorPlaceBrick,
                EditorDeleteBrick,
                EditorTogglePlacementSide,
                EditorUndoRedoHold,
                EditorUndo,
                EditorRedo,
                EditorRotateBrickRight,
                EditorRotateBrickLeft,
                EditorResetShip,
                EditorSaveShip,
                EditorToggleNavigateCategories,
                EditorCameraRotateActive,
                EditorFadeHold,
                EditorFadeSupervisor,
                EditorFadeColors,
                EditorFadeMenu,
                EditorFadeDummy,
                EditorRenameShip,
                EditorAddNewShip,
                EditorDeleteShip,
                NavigationSelect,
                NavigationBack,
                MenuRight,
                MenuLeft,
                GameUseLaser,
                GameUseRocket,
                GameUseMine,
                GameUseMG,
                GameShoot,
                GameShootLaser,
                GameShootRocket,
                GameShootMine,
                GameShootMG,
                GameThrust,
                GameThrustB_1,
                GamePause,
                GameClick,
                Exit,
                LAST = Exit
            };
        };

        // **************************************************************************
        // Values (Mappings)
        // **************************************************************************

        struct SDirectionMapping
        {
            EGamepadDirection::Type Gamepad;
            EKeyboardDirection::Type Keyboard;
        };

        struct SAxisMapping
        {
            EGamepadAxis::Type Gamepad;
            EKeyboardAxis::Type Keyboard;
        };

        struct SButtonMapping
        {
            EGamepadButton::Type Gamepad;
            EKeyboardButton::Type Keyboard;
        };
    }
}
typedef sba::Input::EDirection sba_Direction;
typedef sba::Input::EAxis sba_Axis;
typedef sba::Input::EButton sba_Button;

#endif /* _INPUTMANAGERTYPES_H_ */