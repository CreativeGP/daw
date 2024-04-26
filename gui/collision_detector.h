#ifndef __COLLISION_DETECTOR_H__
#define __COLLISION_DETECTOR_H__

#include "util.h"
#include "component.h"

#include <map>
#include <functional>

#include <SDL2/SDL.h>

typedef enum {
    ORIGIN_PARENT_BOX,
    ORIGIN_SCROLLABLE_WINDOW,
    ORIGIN_SCROLLABLE_Y_ONLY    
} OriginType;

class CollisionDetector : public Component {
public:
    CollisionDetector();
    CollisionDetector(bool fixed);
    ~CollisionDetector();
    
    virtual Message update(GlobalCargo *globals);
    virtual void draw(/*SDL_Surface *surface*/ SDL_Renderer *);

    bool is_dragged() { return this->dragged; }

    std::function<void(GlobalCargo *)> f_on_click;
    std::function<void(GlobalCargo *, int, int)> f_on_drag;
public:
    // 親ウィンドウの位置、inner位置がこのcollのドラッグによって動かされる特殊な場合においてもfixedしたほうが良い
    bool fixed = false;
    // 基準：とりあえず３つ。当たり判定オブジェクトがウィンドウに内包されている場合につかう。
    // ORIGIN_PARENT_BOX　当たり判定は親ウィンドウについていく
    // ORIGIN_SCROLLABLE_WINDOW 当たり判定がスクロール可能なボックスの中にある場合
    // ORIGIN_SCROLLABLE_Y_ONLY 多分ピアノロールでしか使わない...?
    std::pair<OriginType, Component *> origin[3];

private:
    bool dragged = false;
    int drag_delta_x, drag_delta_y;
};

#endif
