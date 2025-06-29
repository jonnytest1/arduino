#include "joystick.h"
#include <TFT_eSPI.h>
#include <stdexcept>
#include <variant>
#include <vector>

using Callback = std::function<void()>;

#ifndef cstpages
#define cstpages

class Page
{

  public:
    virtual String name() = 0;
    virtual bool frame()
    {
        return false;
    }
    bool selected = false;
    TFT_eSprite *screen;

    virtual void onSelected()
    {
    }
    virtual void onUnSelected()
    {
    }

    virtual void draw() = 0;
    virtual ~Page()
    {
        delete screen;
    }
};

class CallbackPage : public Page
{
  private:
    Callback cb;

  public:
    CallbackPage(Callback cb) : cb(cb)
    {
    }
    String name()
    {
        return "callback page";
    }
    void draw()
    {
        cb();
    }
};

class Pages : public EventEmitter
{
    TFT_eSPI *screen;
    int row = 0;
    int col = 0;
    // col > rows

    Page *current;

  public:
    inline static auto PAGE_CHANGE = EventType<bool>::of("page_change");
    bool needsPageUpdate = true;
    std::vector<std::vector<Page *>> pages;
    Pages(Joystick *stick, TFT_eSPI *screen) : screen(screen)
    {

        stick->x_right.on<bool>(BoolChangeRef::TRUE_EVENT, [this](bool val) {
            row++;

            if (row >= pages.at(col).size())
            {
                row = 0;
            }
            needsPageUpdate = true;
        });
        stick->x_left.on<bool>(BoolChangeRef::TRUE_EVENT, [this](bool val) {
            row--;
            if (row < 0)
            {
                row = pages.at(col).size() - 1;
            }
            needsPageUpdate = true;
        });
        stick->y_up.on<bool>(BoolChangeRef::TRUE_EVENT, [this](bool val) {
            ESP_LOGI(L_TAG, "yup");
            col--;
            if (col < 0)
            {
                col = pages.size() - 1;
                if (row >= pages.at(col).size())
                {
                    row = pages.at(col).size() - 1;
                }
            }
            needsPageUpdate = true;
        });
        stick->y_down.on<bool>(BoolChangeRef::TRUE_EVENT, [this](bool val) {
            ESP_LOGI(L_TAG, "ydown");
            col++;
            if (col >= pages.size())
            {
                col = 0;
            }
            if (row >= pages.at(col).size())
            {
                row = pages.at(col).size() - 1;
            }
            needsPageUpdate = true;
        });
    }

    void pageUpdate()
    {

        Page *newPage = pages.at(col).at(row);
        if (current && current != newPage)
        {
            current->selected = false;
            current->onUnSelected();

            if (current->frame() && current->screen)
            {
                delete current->screen;
            }
        }
        current = newPage;
        current->selected = true;

        if (current->frame())
        {

            current->screen = new TFT_eSprite(screen);

            // ESP_LOGI(L_TAG, "sprite");
            int w = screen->width();
            int h = screen->height();
            // ESP_LOGI(L_TAG, "w:%d h:%d iCol:%d r:%d", w, h, col, row);

            if (current->screen->createSprite(w, (h - 8)) == nullptr)
            {

                ESP_LOGE(L_TAG, "create nullptr");
            }
            current->screen->fillSprite(TFT_BLACK);
            current->screen->setTextSize(1);
            current->screen->setTextDatum(TL_DATUM);
            current->screen->setTextColor(TFT_YELLOW, TFT_BLACK);
            current->screen->setTextFont(1);
            // ESP_LOGI(L_TAG, "%d", current->screen->width());

            current->screen->fillSprite(TFT_BLACK);

            // ESP_LOGI(L_TAG, "call selected");
            ESP_LOGI(L_TAG, "%s", current->name());
        }

        current->onSelected();
        emit(Pages::PAGE_CHANGE, true);

        screen->fillScreen(TFT_BLACK);
        screen->drawString(String("page:") + col + "/" + row + ": " + current->name(), 0, 0);
        screen->drawLine(0, 10, screen->width(), 10, TFT_LIGHTGREY);

        ESP_LOGI(L_TAG, "page update done");
    }

    void update()
    {

        if (needsPageUpdate)
        {
            needsPageUpdate = false;
            pageUpdate();
        }

        try
        {

            auto column = pages[col];
            Page *page = column[row];

            page->draw();
            //  TFT_BLACK
            if (current->frame() && page->screen)
            {

                page->screen->pushSprite(0, 12);
            }
        }

        catch (std::out_of_range e)
        {
            ESP_LOGI(L_TAG, "oor");
            //
        }
    }

    void set(int col, int row, Page *cb)
    {
        if (col >= (int)pages.size())
        {
            pages.resize(col + 1); // fill with empty vectors
        }

        auto &column = pages[col];

        if (row > (int)column.size())
        {
            // If you want to allow gaps, you could resize with default values
            column.resize(row); // fill missing with default constructed Callback
        }

        if (row == (int)column.size())
        {
            column.push_back(cb);
        }
        else
        {
            column.insert(column.begin() + row, cb);
        }
    }
};

#endif