# The word clock that took eleven years

I gave my wife a clock tonight. I started building it in August 2015.

It's a wooden panel with English words cut through it, about a hundred LEDs behind the words, and an Arduino that decides which words to light up. Right now it says IT IS QUARTER TO TEN PM, which is correct, which is a minor miracle. On May 4th it'll light up rainbow LEDs for HAPPY BIRTH DAY CHELSEA, her name and her birthday. That's been the point of this project since before we were married.

## 2015

I'd graduated from UCSB the year before with a CS degree. I had written enough C++ to pass my classes and not enough to remember any of it. I'd never touched an Arduino. I knew what a resistor was the way I know what a differential is on a car: I knew the word.

Then I saw a Reddit post. Someone had built a word clock from scratch because the commercial version cost $500 and he didn't want to pay $500. My buddy Perry and I decided, with no evidence of our own competence, that we'd each build one for our girlfriends. We drove to the UCSB electrical engineering shop for chips and resistors.

The clever part of a word clock is the letter grid. Every time phrase — IT IS TWENTY FIVE PAST SEVEN — has to read in a straight line, and the letters you aren't using have to look like filler. I started from the Reddit grid and bent it until HAPPY, BIRTH, DAY, and CHELSEA all fit in the leftover letters. That was the real project. The clock was the packaging.

I sent the layout to some guy on Etsy who owned a laser cutter. He mailed me back a beautiful wooden panel. I never met him.

## The wall

Here's the problem with an Arduino Uno: 14 digital pins. Here's the problem with a word clock: it needs 28 signals. The math doesn't work, which means you can't just tell a pin "be on" and have a word light up. You need something cleverer.

The Reddit post used shift registers — little chips that let you multiplex more outputs than you have pins. Reasonable technique. For me in 2015, a wall.

My code compiled. My LEDs lit. Sometimes the right ones. Sometimes the right ones for three seconds before the whole thing fell apart. I didn't own a logic analyzer. I didn't really know what one was. My debugging method was to change a number and see if the clock felt better about it. It did not.

I got it, generously, 90% of the way there. Summer ended. School started. I put the whole thing in a box.

## The box

Eleven years.

I moved apartments and the box came with me. I moved cities. It came. I married the girl whose birthday was the whole point. She knew what was in the box. I knew she knew. We never talked about it. Not once.

## The revival

This spring I had a month between jobs. I also had Claude Code, which knows Arduino better than I ever will. The friction that killed the 2015 build — sit down, remember what a shift register is, read a datasheet, fail to debug it — is exactly the friction that's gotten cheap in the last year. I pulled the box out of the garage.

What followed was two evenings of confidently debugging in the wrong direction.

According to every note and code comment I'd written, the board was wired "common anode" — positive side shared across all the LEDs, each word switched on by grounding it through a transistor. Fine. Documented. I bought seven transistor boards. Planned the mounting. Picked wire colors. Wired it up.

Nothing lit.

Probed voltages. Fine. Probed continuity. Fine. Poked a wire with my finger and three LEDs flickered for a heartbeat, and I built an entire theory around that flicker. The theory was wrong. I spent a whole session chasing it.

Eventually I went back to basics. Grabbed one LED. Put 5V on one leg, ground on the other. It lit. Swapped the legs. It didn't. And then I finally understood: the big shared wire on the board wasn't the positive bus. It was ground. The whole thing had been wired the opposite of what the docs said since 2015. I'd been trying to push current backwards through every LED on the panel.

I walked around the block.

## The fix

When I came back I asked Claude the question I should've asked on day one: what's the simplest possible way to finish this?

The Arduino I'd bought to replace the Uno was a Mega. 54 pins. I needed 28. I didn't need the transistors. I could solder each word's wire straight to an Arduino pin, call `digitalWrite(pin, HIGH)`, and the word would light. That's the whole circuit.

The thing that killed me in 2015 was trying to drive 28 words through 14 pins. The fix, eleven years later, was a board with more pins. I'd been solving the wrong problem the entire time.

The transistor boards went back to Amazon. From "wait, it's common cathode" to "IT IS HALF PAST TWO PM" on the wall: about an hour.

## One more thing

The clock showed a correct time and then refused to advance. I added a diagnostic print and found the RTC module from 2015 was running at 43% of real time. Eleven years in a closet had killed the crystal. I ordered a new one. In the meantime I patched the code to keep time using the Arduino's own clock, which forgets the time on power loss but holds fine for a couple days.

## Tonight

She has the clock. It's on the wall by the kitchen, saying the time in warm white words, the letters it isn't using disappearing into the wood. When the new RTC gets here Tuesday I'll set the date, and on May 4th the birthday LEDs will light up in rainbow. She hasn't seen that mode yet. I'm looking forward to her face.

Eleven years late, which by some measures is a long time, and by the measure of a marriage isn't that long.

## The lesson, if there is one

What I'd tell 2015 me is that he was solving the wrong problem. He was fighting shift registers because the Reddit post used shift registers, and the Reddit post used shift registers because everyone uses a Uno, and everyone uses a Uno because everyone uses a Uno. One shelf over was a bigger board with enough pins. He would've finished the clock that summer.

And: don't trust what your past self wrote down. "Common anode" had been in the docs for eleven years. It was always wrong. Every document agreed because every document was copied from the same bad sentence. Two evenings of debugging dissolved the second we put a probe on one LED.

Check the thing.

## The clock

It's on the wall. It says the time. She smiled.

That was the whole project.
