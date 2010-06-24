/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of mhome.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef SWITCHERBUTTON_H
#define SWITCHERBUTTON_H

#include <QTimer>
#include <MButton>
#include "switcherbuttonmodel.h"
#include "windowinfo.h"
#include <X11/Xlib.h>

/*!
 * SwitcherButton widget represents an open window in the Switcher.
 */
class SwitcherButton : public MButton
{
    Q_OBJECT
    M_CONTROLLER(SwitcherButton)

public:
    /*!
     * Constructs an SwitcherButton.
     *
     * \param title the Title of the window represented by this button
     * \param parent parent MWidget
     * \param window the X Window represented by this button
     */
    SwitcherButton(const QString &title, MWidget *parent = NULL, Window window = 0);

    /*!
     * Destroys the SwitcherButton.
     */
    virtual ~SwitcherButton();

    /*!
     * Gets the X Window represented by this button
     */
    Window xWindow();

    /*! \reimp
     *
     * Reimplemented here as public because it's protected in the base class so the view cannot access it.
     */
    virtual void prepareGeometryChange();
    virtual void setGeometry(const QRectF &rect);
    //! \reimp_end

    //! Sets/unsets _MEEGO_VISIBLE_IN_SWITCHER property to the window.
    void setVisibleInSwitcherProperty(bool set);

signals:
    /*!
     * \brief A signal for notifying that a window should be brought to front
     */
    void windowToFront(Window window);

    /*!
     * \brief A signal for notifying that the window should be closed
     */
    void closeWindow(Window window);

public slots:
    /*!
     * \brief Slot for notifying that the window represented by this button should be brought to front
     */
    void switchToWindow();

    /*!
     * \brief Slot for notifying that the window represented by this button should be closed
     */
    void close();

    /*!
     * A slot that should be called when window closing has been requested but the window has not closed during a certain time.
     */
    void resetState();

    /*!
     * \brief Updates the _NET_WM_ICON_GEOMETRY X property for the Window
     */
    void updateIconGeometry();

protected:

    /*! \reimp
     *
     * Reimplemented to update _MEEGO_VISIBLE_IN_SWITCHER property
     */
    virtual void enterDisplayEvent();

    /*! \reimp
     *
     * Reimplemented to update _MEEGO_VISIBLE_IN_SWITCHER property
     */
    virtual void exitDisplayEvent();

private:

    //! A timer to check if the window is closed when switcher button is dragged outside switcher before timer expires
    QTimer windowCloseTimer;

    //! X11 Atom for the icon geometry
    static Atom iconGeometryAtom;

    //! X11 Atom for window being visible in the switcher
    static Atom visibleAtom;
};

#endif // SWITCHERBUTTON_H
