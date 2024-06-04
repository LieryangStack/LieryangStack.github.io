import gi
import inspect
gi.require_version('Adw', '1')
from gi.repository import Adw

def on_activate(app):
    win = Adw.ApplicationWindow(application=app)
    win.present()

print (inspect.getfile(gi))

app = Adw.Application()
app.connect('activate', on_activate)

app.run(None)
