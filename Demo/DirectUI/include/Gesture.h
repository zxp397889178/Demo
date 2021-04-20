#pragma once

#if(WINVER < 0x0601)

#define WM_GESTURE                      0x0119
#define WM_GESTURENOTIFY                0x011A

/*
* Gesture defines and functions
*/

/*
* Gesture information handle
*/
DECLARE_HANDLE(HGESTUREINFO);


/*
* Gesture flags - GESTUREINFO.dwFlags
*/
#define GF_BEGIN                        0x00000001
#define GF_INERTIA                      0x00000002
#define GF_END                          0x00000004

/*
* Gesture IDs
*/
#define GID_BEGIN                       1
#define GID_END                         2
#define GID_ZOOM                        3
#define GID_PAN                         4
#define GID_ROTATE                      5
#define GID_TWOFINGERTAP                6
#define GID_PRESSANDTAP                 7
#define GID_ROLLOVER                    GID_PRESSANDTAP

/*
* Gesture information structure
*   - Pass the HGESTUREINFO received in the WM_GESTURE message lParam into the
*     GetGestureInfo function to retrieve this information.
*   - If cbExtraArgs is non-zero, pass the HGESTUREINFO received in the WM_GESTURE
*     message lParam into the GetGestureExtraArgs function to retrieve extended
*     argument information.
*/
typedef struct tagGESTUREINFO {
	UINT cbSize;                    // size, in bytes, of this structure (including variable length Args field)
	DWORD dwFlags;                  // see GF_* flags
	DWORD dwID;                     // gesture ID, see GID_* defines
	HWND hwndTarget;                // handle to window targeted by this gesture
	POINTS ptsLocation;             // current location of this gesture
	DWORD dwInstanceID;             // internally used
	DWORD dwSequenceID;             // internally used
	ULONGLONG ullArguments;         // arguments for gestures whose arguments fit in 8 BYTES
	UINT cbExtraArgs;               // size, in bytes, of extra arguments, if any, that accompany this gesture
} GESTUREINFO, *PGESTUREINFO;
typedef GESTUREINFO const * PCGESTUREINFO;


/*
* Gesture notification structure
*   - The WM_GESTURENOTIFY message lParam contains a pointer to this structure.
*   - The WM_GESTURENOTIFY message notifies a window that gesture recognition is
*     in progress and a gesture will be generated if one is recognized under the
*     current gesture settings.
*/
typedef struct tagGESTURENOTIFYSTRUCT {
	UINT cbSize;                    // size, in bytes, of this structure
	DWORD dwFlags;                  // unused
	HWND hwndTarget;                // handle to window targeted by the gesture
	POINTS ptsLocation;             // starting location
	DWORD dwInstanceID;             // internally used
} GESTURENOTIFYSTRUCT, *PGESTURENOTIFYSTRUCT;

/*
* Gesture argument helpers
*   - Angle should be a double in the range of -2pi to +2pi
*   - Argument should be an unsigned 16-bit value
*/
#define GID_ROTATE_ANGLE_TO_ARGUMENT(_arg_)     ((USHORT)((((_arg_) + 2.0 * 3.14159265) / (4.0 * 3.14159265)) * 65535.0))
#define GID_ROTATE_ANGLE_FROM_ARGUMENT(_arg_)   ((((double)(_arg_) / 65535.0) * 4.0 * 3.14159265) - 2.0 * 3.14159265)

/*
* Gesture information retrieval
*   - HGESTUREINFO is received by a window in the lParam of a WM_GESTURE message.
*/

typedef struct tagGESTURECONFIG {
	DWORD dwID;                     // gesture ID
	DWORD dwWant;                   // settings related to gesture ID that are to be turned on
	DWORD dwBlock;                  // settings related to gesture ID that are to be turned off
} GESTURECONFIG, *PGESTURECONFIG;

/*
* Gesture configuration flags - GESTURECONFIG.dwWant or GESTURECONFIG.dwBlock
*/

/*
* Common gesture configuration flags - set GESTURECONFIG.dwID to zero
*/
#define GC_ALLGESTURES                              0x00000001

/*
* Zoom gesture configuration flags - set GESTURECONFIG.dwID to GID_ZOOM
*/
#define GC_ZOOM                                     0x00000001

/*
* Pan gesture configuration flags - set GESTURECONFIG.dwID to GID_PAN
*/
#define GC_PAN                                      0x00000001
#define GC_PAN_WITH_SINGLE_FINGER_VERTICALLY        0x00000002
#define GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY      0x00000004
#define GC_PAN_WITH_GUTTER                          0x00000008
#define GC_PAN_WITH_INERTIA                         0x00000010

/*
* Rotate gesture configuration flags - set GESTURECONFIG.dwID to GID_ROTATE
*/
#define GC_ROTATE                                   0x00000001

/*
* Two finger tap gesture configuration flags - set GESTURECONFIG.dwID to GID_TWOFINGERTAP
*/
#define GC_TWOFINGERTAP                             0x00000001

/*
* PressAndTap gesture configuration flags - set GESTURECONFIG.dwID to GID_PRESSANDTAP
*/
#define GC_PRESSANDTAP                              0x00000001
#define GC_ROLLOVER                                 GC_PRESSANDTAP

#define GESTURECONFIGMAXCOUNT           256             // Maximum number of gestures that can be included
// in a single call to SetGestureConfig / GetGestureConfig
#endif

