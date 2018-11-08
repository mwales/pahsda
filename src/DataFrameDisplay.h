#ifndef DATAFRAMEDISPLAY_H
#define DATAFRAMEDISPLAY_H

/**
 * Inherit from a QLabel.  Set the text as rich text, font monospaced.  We can do advanced text
 * formatting with HTML and CSS tags of the data.  Have a QTimer internal that fires at 1 Hz to
 * gradually remove highlight effects over time.  Have a way to pause this timer too.
 */
class DataFrameDisplay
{
public:
   DataFrameDisplay();
};

#endif // DATAFRAMEDISPLAY_H
