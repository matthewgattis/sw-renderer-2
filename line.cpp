#include "line.hpp"

#include <ios>
#include <memory>

#include "framebuffer.hpp"

#define ABS(x) ( ((x) < 0) ? -(x) : (x) ) // absolute value

Line::Line(
    const std::shared_ptr<Framebuffer> &framebuffer)
    : framebuffer_(framebuffer)
{
}

void Line::draw(
    const glm::dvec2 &p1,
    const glm::dvec2 &p2,
    const glm::dvec3 &c)
{
  int dX, dY, row, col, final, G, inc1, inc2;
  char pos_slope;

  dX = x2 - x1;
  dY = y2 - y1;
  pos_slope = (dX > 0);
  if (dY < 0)
    pos_slope = !pos_slope;

  if (ABS(dX) > ABS(dY)) {
    if (dX > 0) {
      col = x1;
      row = y1;
      final = x2;
    } else {
      col = x2;
      row = y2;
      final = x1;
    }

    inc1 = 2 * ABS(dY);
    G = inc1 - ABS(dX);
    inc2 = 2 * (ABS(dY) - ABS(dX));
    if (pos_slope) {
      while (col <= final) {
        dotproc(col, row);
        col++;
        if (G >= 0) {
          row++;
          G += inc2;
        } else
          G += inc1;
      }
    } else {
      while (col <= final) {
        dotproc(col, row);
        col++;
        if (G > 0) {
          row--;
          G += inc2;
        } else
          G += inc1;
      }
    }
  } /* if |dX| > |dY| */
  else {
    if (dY > 0) {
      col = x1;
      row = y1;
      final = y2;
    } else {
      col = x2;
      row = y2;
      final = y1;
    }

    inc1 = 2 * ABS(dX);
    G = inc1 - ABS(dY);
    inc2 = 2 * (ABS(dX) - ABS(dY));

    if (pos_slope) {
      while (row <= final) {
        dotproc(col, row);
        row++;
        if (G >= 0) {
          col++;
          G += inc2;
        } else
          G += inc1;
      }
    } else {
      while (row <= final) {
        dotproc(col, row);
        row++;
        if (G > 0) {
          col--;
          G += inc2;
        } else
          G += inc1;
      }
    }
  }
}

