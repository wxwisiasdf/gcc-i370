/* Copyright (C) 1999, 2000  Free Software Foundation

   This file is part of libgcj.

This software is copyrighted work licensed under the terms of the
Libgcj License.  Please consult the file "LIBGCJ_LICENSE" for
details.  */

package java.awt;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.peer.WindowPeer;
import java.awt.peer.ComponentPeer;
import java.util.EventListener;
import java.util.Locale;
import java.util.ResourceBundle;

/* Status: partially implemented. */

public class Window extends Container
{
  // Serialized fields, from Sun's serialization spec.
  // private FocusManager focusMgr;  // FIXME: what is this?  
  private String warningString = null;
  private int state = 0;
  private int windowSerializedDataVersion = 0; // FIXME

  private transient WindowListener windowListener;
  private transient GraphicsConfiguration graphicsConfiguration;

  public Window(Frame owner)
  {
    this (owner, null);
  }

  /** @since 1.2 */
  public Window(Window owner)
  {
    this (owner, null);
  }
  
  /** @since 1.3 */
  public Window(Window owner, GraphicsConfiguration gc)
  {
    /*  FIXME: Security check
    SecurityManager.checkTopLevelWindow(...)

    if (gc != null
        && gc.getDevice().getType() != GraphicsDevice.TYPE_RASTER_SCREEN)
      throw new IllegalArgumentException ("gc must be from a screen device");

    if (gc == null)
      graphicsConfiguration = GraphicsEnvironment.getLocalGraphicsEnvironment()
			     .getDefaultScreenDevice()
			     .getDefaultConfiguration();
    else
    */    
      graphicsConfiguration = gc;

    // FIXME: compiler bug
    // this.layoutMgr = new BorderLayout ();
    
    if (owner == null)
      throw new IllegalArgumentException ("Owner can not be null");
          
    this.parent = owner;
    
    // FIXME: add to owner's "owned window" list
  }

  protected void finalize() throws Throwable
  {
    // FIXME: remove from owner's "owned window" list (Weak References)
  }

  public void addNotify()
  {
    if (peer == null)
      // FIXME: This cast should NOT be required. ??? Compiler bug ???
      peer = (ComponentPeer) getToolkit ().createWindow (this);
  }

  /** @specnote pack() doesn't appear to be called internally by show(), so
                we duplicate some of the functionality. */
  public void pack()
  {
    if (parent != null
        && !parent.isDisplayable())
      parent.addNotify();
        if (peer == null)
      addNotify();
    
    // FIXME: do layout stuff here
    
    validate();
  }

  public void show ()
  {
    if (isVisible())
      {
	this.toFront();
	return;
      }
  
    if (parent != null
        && !parent.isDisplayable())
      parent.addNotify();
    if (peer == null)
      addNotify ();    

    validate ();
    
    super.show ();

    // FIXME: Is this call neccessary or do we assume the peer takes care of 
    // it?
    // this.toFront();
  }

  public void hide()
  {
    // FIXME: call hide() on amy "owned" children here.
    super.hide();
  }

  public void dispose()
  {
    // FIXME: first call removeNotify() on owned children
    for (int i = 0; i < ncomponents; ++i)
      component[i].removeNotify();
    this.removeNotify();
  }

  public void toBack ()
  {
    if (peer != null)
      {
	WindowPeer wp = (WindowPeer) peer;
	wp.toBack ();
      }
  }

  public void toFront ()
  {
    if (peer != null)
      {
	WindowPeer wp = (WindowPeer) peer;
	wp.toFront ();
      }
  }

  public Toolkit getToolkit()
  {
    // FIXME: why different from Component.getToolkit() ?
    return super.getToolkit();
  }

  public final String getWarningString()
  {
    boolean secure = true;
    /* boolean secure = SecurityManager.checkTopLevelWindow(...) */

    if (!secure)
      {
        if (warningString != null)
	  return warningString;
	else
	  {
	    String warning = System.getProperty("awt.appletWarning");
	    return warning;
	  }
      }
    return null;
  }

  public Locale getLocale ()
  {
    return locale == null ? Locale.getDefault () : locale;
  }

  /*
  /** @since 1.2
  public InputContext getInputContext()
  {
    // FIXME
  }
  */

  public void setCursor(Cursor cursor)
  {
    // FIXME: why different from Component.setCursor() ?
    super.setCursor(cursor);
  }

  public Window getOwner()
  {
    if (parent != null)
      return (Window) parent;
    else 
      return null;
  }

  /** @since 1.2 */
  public Window[] getOwnedWindows()
  {
    // FIXME: return array containing all the windows this window currently 
    // owns.
    return null;
  }

  public synchronized void addWindowListener (WindowListener listener)
  {
    windowListener = AWTEventMulticaster.add (windowListener, listener);
  }

  public synchronized void removeWindowListener (WindowListener listener)
  {
    windowListener = AWTEventMulticaster.remove (windowListener, listener);
  }

  /** @since 1.3 */
  public EventListener[] getListeners(Class listenerType)
  {
    if (listenerType == WindowListener.class)
      return getListenersImpl(listenerType, windowListener);
    else return super.getListeners(listenerType);
  }

  void dispatchEventImpl(AWTEvent e)
  {
    // Make use of event id's in order to avoid multiple instanceof tests.
    if (e.id <= WindowEvent.WINDOW_LAST 
        && e.id >= WindowEvent.WINDOW_FIRST
        && (windowListener != null 
	    || (eventMask & AWTEvent.WINDOW_EVENT_MASK) != 0))
      processEvent(e);
    else
      super.dispatchEventImpl(e);
  }

  protected void processEvent (AWTEvent evt)
  {
    if (evt instanceof WindowEvent)
      processWindowEvent ((WindowEvent) evt);
    else
      super.processEvent (evt);
  }

  protected void processWindowEvent (WindowEvent evt)
  {
    if (windowListener != null)
      {
	switch (evt.getID ())
	  {
	  case WindowEvent.WINDOW_ACTIVATED:
	    windowListener.windowActivated (evt);
	    break;
	  case WindowEvent.WINDOW_CLOSED:
	    windowListener.windowClosed (evt);
	    break;
	  case WindowEvent.WINDOW_CLOSING:
	    windowListener.windowClosing (evt);
	    break;
	  case WindowEvent.WINDOW_DEACTIVATED:
	    windowListener.windowDeactivated (evt);
	    break;
	  case WindowEvent.WINDOW_DEICONIFIED:
	    windowListener.windowDeiconified (evt);
	    break;
	  case WindowEvent.WINDOW_ICONIFIED:
	    windowListener.windowIconified (evt);
	    break;
	  case WindowEvent.WINDOW_OPENED:
	    windowListener.windowOpened (evt);
	    break;
	  }
      }
  }

  public Component getFocusOwner()
  {
    // FIXME
    return null;
  }

  public boolean postEvent(Event e)
  {
    // FIXME
    return false;
  }

  public boolean isShowing()
  {
    // FIXME: Also check if window is within the boundary of the screen?
    return isVisible();
  }

  /** @since 1.2 */
  public void applyResourceBundle(ResourceBundle rb)
  {
    // FIXME
  }

  /** @since 1.2 */
  public void applyResourceBundle(String rbName)
  {
    ResourceBundle rb = ResourceBundle.getBundle(rbName);
    if (rb != null)
      applyResourceBundle(rb);    
  }

  /*
  public AccessibleContext getAccessibleContext()
  {
    // FIXME
  }
  */

  public GraphicsConfiguration getGraphicsConfiguration()
  {
    return graphicsConfiguration;
  }
}
