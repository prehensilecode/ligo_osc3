// Yo, Emacs! This is -*- LaTeX -*-
//
// $Id: users_guide.hh,v 1.6 2003/01/25 03:15:55 dwchin Exp $
//
// Contact: David Chin <dwchin@umich.edu>
//
//         2477 Randall Lab, Univ. of Michigan
//         500 E. University Ave.
//         Ann Arbor, MI 48109
//         +1-734-764-5146
//         +1-734-730-1274
//
// Original author: K. Riles <kriles@umich.edu>



//@{

/** @name User's Guide
\begin{center}
  \textbf{{\Large Defining and Testing Operational State Conditions 
      in the Data Monitor Tool}}
\end{center}

\begin{center}
  \textbf{{\large Abstract}}
\end{center}

This document is a user's guide to defining and testing Operational
State Conditions (OSCs) in the Data Monitor Tool (DMT) background
environment.  It is assumed the reader knows the rudiments of creating
a background monitor in the DMT using the \texttt{DatEnv} class.

\begin{center}
  \textbf{{\large Features}}
\end{center}

\begin{itemize}
\item \texttt{include} directive to include standard pre-defined
  conditions, \textit{e.g.} one can \texttt{include} the configuration
  that is currently used by a running monitor, such as
  \texttt{LockLoss}, and thus use the same conditions the monitor is
  using.
\item Ability to combine multiple conditions into one compound
  condition using Boolean logic.
\item Ability to read parameter values from EPICS channels, giving
  real-time control of parameters.
\item Access to values that are being monitored via the OSC object
  itself. (For instantaneous values, only one datapoint from all the
  data in the time stride is returned.)
\item Optional parameters need not be specified in configuration
  file: default values are pre-defined.
\item Inline comments
\end{itemize}

\begin{center}
  \textbf{{\large Introduction}}
\end{center}

Before undertaking an analysis, one often needs to specify required
interferometer or environment conditions.  For example, before looking
for instability in the Recycling Mirror (RM) servo, one would likely
require both that servo and the Beam Splitter (BS) servo to be locked.
One might also want Wave Front Sensing (WFS) to be engaged.  In
another analysis, one might require the laser intensity to exceed a
threshold or require seismic motion to fall below a ceiling. To allow
for flexible and comprehensive setting of conditions, one would also
like to specify Boolean combinations of conditions. For example

\begin{verbatim}
    Cond_1 = RM servo locked
    Cond_2 = BS servo locked
    Cond_3 = Cond_1 & Cond_2 
\end{verbatim}

The OSC tool has been written to support convenient defining and
run-time checking of such conditions. Definitions are made via a
text configuration file. A handful of preliminary ``standard'' 
conditions (e.g., ``\texttt{Arm\_locked}'' and ``\texttt{LVEA\_quiet}'') 
have been provided in a sample configuration file.  It is
expected that many more such standard conditions will be defined and
refined as physicists gain more experience with his and future
datasets. Definitions for useful conditions should be sent to the
author (\texttt{dwchin@umich.edu}) for incorporation into a public
repository.

\begin{center}
  \textbf{\large Quick Sample of Code}
\end{center}

Before detailing how to use the OSC tool, let's first get a flavor of
it with a stripped-down sample of code. The following code defines
conditions and checks them in real time:

\begin{verbatim}
    // During initialization:
    osclist.readConfig("osc_sample.config");
  
    // As each time interval of data (e.g., 1-second frame) is read: 
    if ( osclist.satisfied("LVEA_quiet") ) {
      // Do some analysis....
    }
\end{verbatim}

One can also loop through \textit{all} OSCs defined in a configuration
file using an iterator:
\begin{verbatim}
    // During initialization:
    osclist.readConfig("osc_sample.config");

    // As each time interval of data is read:
    OperStateCondList::iterator iter = osclist.begin();
    for (; iter != osclist.end(); ++iter) {

        // print out value being monitored. (does NOT work for meta-OSCs)
        cout << "'" << (*iter).first << "': watched value = "
             << (*iter).second->watchedQuantity() << endl;

        if ((*iter).second->satisfied() == true)
            cout << "'" << (*iter).first << "'\tsatisfied" << endl;
        else 
            cout << "'" << (*iter).first << "'\t\tNOT satisfied" 
                 << endl;
    }
\end{verbatim}
This scrap of code assumes that the monitor class has been written
with \texttt{DatEnv} as a base class, and \texttt{osclist} is a member 
\texttt{OperStateCondList} object.

The \texttt{readConfig()} call is made in the monitor's constructor. The
\texttt{satisfied()} call is made from the monitor's \texttt{ProcessData}
method. The configuration file might look like:

\begin{verbatim}
    # A comment
    x_quiet    rmsrange "H0:PEM-LVEA_SEISX" lo=0. hi=2000.
    y_quiet    rmsrange "H0:PEM-LVEA_SEISY" lo=0. hi=2000.
    z_quiet    rmsrange "H0:PEM-LVEA_SEISZ" lo=0. hi=2000.
    LVEA_quiet boolean  "x_quiet & y_quiet & z_quiet"   # a comment

    # in general, lines look like the following, where
    # "OscName", osc_type, "paramname" are case-insensitive,
    # i.e. FooBar, fOObAR and foobar are all equivalent names
    OscName  osc_type "channel_name" paramname=paramvalue  ...
\end{verbatim}

The first three conditions have the names ``\texttt{x\_quiet}'',
``\texttt{y\_quiet}'' and ``\texttt{z\_quiet}''. Each is of type
\texttt{rmsrange}
which means the data channel with the specified name must have an RMS
value between the minimum and maximum values specified (0. and
2000. in each case).  The fourth condition is Boolean, in this case
the logical AND of the first three conditions.

Fields are separated by spaces or tabs.  Valid condition names must begin
with an alphabetic character and contain only alphanumeric characters,
``\_'' (underscore), or ``:'' (colon). Condition names are case-insensitive, 
\textit{i.e.} \texttt{FooBar}, \texttt{fOObAR}, and \texttt{foobar} are 
all equivalent.  For \texttt{boolean} conditions, spaces are not required 
in the Boolean expression. The bitmasks can be specified in decimal, octal, 
and hexadecimal format: octal numbers are denoted by a leading ``0'' (zero) 
and hexadecimal numbers are denoted by a leading ``0x'' (zero x).

\begin{center}
  \textbf{{\large Other Directives}}
\end{center}

There are other directives that can be specified in the configuration
file which are not directly involved in defining OSCs.

{\large\texttt{\textbf{include}}}
\begin{itemize}
\item \textbf{Effect:} Similar to ``\texttt{\#include}'' for the C
  preprocessor, it includes other configuration files in line. If the
  file is not found, a warning message is printed and the directive is
  ignored:  processing continues.
\item \textbf{Syntax:} There are two ways of using the
  \texttt{include} directive:
  \begin{itemize}
  \item \texttt{include "otherFile.conf"} includes a configuration
    file in the same directory as the current file
  \item \texttt{include <otherFile.conf>} includes a configuration
    file from the directory given by the environment variable
    \texttt{DMTPARS}.  If the environment variable is not set, 
    a warning message is printed and the directive is ignored:
    processing continues.
  \end{itemize}
\end{itemize}

{\large\texttt{\textbf{ignore}}}
\begin{itemize}
\item \textbf{Effect:} Deletes the named OSC from the
  \texttt{OperStateCondList}.
\item \textbf{Syntax:} \texttt{ignore "previouslyDefinedOSCname"}
\end{itemize}

{\large\texttt{\textbf{ignoreallexcept}}}
\begin{itemize}
\item \textbf{Effect:} Ignores all OSCs that depend on data channels
  with names beginning with the given prefix.
\item \textbf{Syntax:} \texttt{ignoreallexcept "H1:LSC"}
\item \textbf{Note:} This can be dangerous, especially if you have
  dependencies on OSCs that you do not name.  The only way to do that 
  would be to write your own code in your monitor.  Wouldn't it be
  nice to have regular expressions? 
\end{itemize}

{\large\texttt{\textbf{debuglevel}}}
\begin{itemize}
\item \textbf{Effect:} Sets the debug level of the
  \texttt{OperStateCondList}.
\item \textbf{Syntax:} \texttt{debuglevel 2} -- the argument must be an
  integer. Only values $\geq 0$ make sense, though negative values do not
  cause an error.
\end{itemize}

{\large\texttt{\textbf{stride}}}
\begin{itemize}
\item \textbf{Effect:} Sets the time stride of the \texttt{Dacc} data
  accessor object associated with the \texttt{OperStateCondList}.
\item \textbf{Syntax:} \texttt{stride 2} -- the argument must be an
  integer. Only values $\gt 0$ make sense, though non-positive values do
  not cause an error.
\item \textbf{Note:} To ensure that the stride that is set from the
  configuration file is in effect, the \texttt{Dacc::setStride()}
  method must \textit{not} be used.
\end{itemize}

\begin{center}
\textbf{{\large Conditions}}
\end{center}

The following list of OSC types are expected to expand with future
releases of the OSC tool.

There are two types of OSCs: atomic OSCs, and meta-OSCs.  The atomic
OSCs specify conditions on the data stream, whereas the meta-OSCs
specify conditions on other OSCs.  Each OSC has some number of
parameters which specify the details of the condition, \textit{e.g.}
threshold, frequency band, etc.

The atomic OSCs require a channel name in the configuration file
definition, while the meta-OSCs require another OSC name in the
definition.  The OSC upon which a meta-OSC acts must have been defined
before the meta-OSC.

Some classes of OSCs have parameters which others do not, \textit{e.g.}
OSCs which place conditions on each sample in the time-series data
have a \texttt{fraction} parameter which specifies the minimum fraction
of data that must satisfy the condition for the OSC to be satisfied.
The \texttt{fraction} parameter would make sense for something like 
\texttt{valueabove} but not for something like \texttt{meanabove}.

Some parameters are \textit{optional}, meaning that they need not be
specified explicitly in the configuration file.  If a parameter is
optional it always has some default value.

If a line defining an OSC in the configuration file contains
parameters that are not part of that OSC or meta-OSC, an undefined
error may result.

\begin{center}
  \textbf{{\large Condition Names}}
\end{center}

All conditions must be named.  Legal names must begin with an alphabetic 
character and may consist of letters, numbers, underscores and colons.  
Names are \textit{case-insensitive}.

\begin{center}
  \textbf{Common Parameters}
\end{center}

This is a list of names of parameters which are used for more than on
OSC type.  Note that $\infty$ is to be interpreted as the limit of the
computer representation of the appropriate data type.  A \textit{stride}
corresponds to one Frame of data. (FIXME)

{\large\texttt{\textbf{fraction}}}
\begin{itemize}
\item \textbf{Definition:} Specifies the fraction of the time-series data
  that  must satisfy the condition for the OSC to be satisfied
\item \textbf{OSCs:} \texttt{valueabove}, \texttt{valuebelow}, \texttt{
    valuerange}, \texttt{bitand}, \texttt{bitnand}, \texttt{bitor}, \texttt{
    bitnor} 
\item \textbf{Optional:} Yes
\item \textbf{Data type:} Floating point  
\item \textbf{Legal values:} $[0, 1]$ Zero means that the OSC is
  satisfied if at least one data point satisfies the condition
\item \textbf{Default value:} $0$
\end{itemize}

{\large\texttt{\textbf{hold}}}
\begin{itemize}
\item \textbf{Definition:} Specifies the number of additional strides for
  which to hold the \texttt{OSC.satisfied()} at True.
\item \textbf{OSCs:} \texttt{transitup}, \texttt{transitdown}
\item \textbf{Optional:} Yes
\item \textbf{Data type:} Integer
\item \textbf{Legal values:} $[0, \infty)$ The OSC will always be true
  for at least one (1) stride, and will be held True for an
  additional \texttt{hold} stride.
\item \textbf{Default value:} $0$
\end{itemize}

{\large\texttt{\textbf{dead}}}
\begin{itemize}
\item \textbf{Definition:} Specifies the number of strides beyond the end
  of a \texttt{hold} period for which the OSC cannot be True
\item \textbf{OSCs:} \texttt{transitup}, \texttt{transitdown}
\item \textbf{Optional:} Yes
\item \textbf{Data type:} Integer
\item \textbf{Legal values:} $[0, \infty)$ The OSC will be held False for
  \texttt{dead} strides
\item \textbf{Default value:} $0$
\end{itemize}

{\large\texttt{\textbf{threshold}}}
\begin{itemize}
\item \textbf{Definition:} Specifies an amplitude threshold for
  \texttt{*above}, and \texttt{*below}, and a change threshold for
  \texttt{*rise}, and \texttt{*fall} OSCs
\item \textbf{OSCs:} \texttt{valueabove}, \texttt{valuebelow},
  \texttt{meanabove}, \texttt{meanbelow}, \texttt{rmsabove}, 
  \texttt{rmsbelow}, \texttt{abspowerabove}, \texttt{abspowerbelow}, 
  \texttt{abspowerrise}, \texttt{abspowerfall},
  \texttt{abspowergain},  
  \texttt{fractpowerabove}, \texttt{fractpowerbelow}
\item \textbf{Optional:} No
\item \textbf{Data type:} Floating point
\item \textbf{Legal values:} $(-\infty, \infty)$
\item \textbf{Default value:} N/A
\end{itemize}

{\large\texttt{\textbf{nstrides}}}
\begin{itemize}
\item \textbf{Definition:} Specifies the number of strides over which
  an average is to be computed
\item \textbf{OSCs:} \texttt{abspowerrise},
  \texttt{abspowerfall}, \texttt{abspowergain}, \texttt{meanrise},
  \texttt{meanfall}
\item \textbf{Optional:} Yes
\item \textbf{Data type:} Integer
\item \textbf{Legal values:} $[1, \infty)$
\item \textbf{Default value:} 1
\end{itemize}

{\large\texttt{\textbf{lo}}}
\begin{itemize}
\item \textbf{Definition:} Specifies the lower limit for \texttt{*range}
  OSCs.  OSC will be satisfied if \texttt{fraction} of data lies between
  lower and upper limits (see \texttt{hi})
\item \textbf{OSCs:} \texttt{valuerange}, \texttt{meanrange},
  \texttt{rmsrange}, \texttt{fractpowerrange}
\item \textbf{Optional:} No
\item \textbf{Data type:} Floating point
\item \textbf{Legal values:} $(-\infty, \infty)$
\item \textbf{Default value:} N/A
\end{itemize}

{\large\texttt{\textbf{hi}}}
\begin{itemize}
\item \textbf{Definition:} Specifies the upper limit for \texttt{*range}
  OSCs.  OSC will be satisfied if \texttt{fraction} of data lies between
  lower and upper limits (see \texttt{lo})
\item \textbf{OSCs:} \texttt{valuerange}, \texttt{meanrange},
  \texttt{rmsrange}, \texttt{fractpowerrange}  
\item \textbf{Optional:} No
\item \textbf{Data type:} Floating point
\item \textbf{Legal values:} $(-\infty, \infty)$
\item \textbf{Default value:} N/A  
\end{itemize}

{\large\texttt{\textbf{freqlo}}}
\begin{itemize}
\item \textbf{Definition:} Specifies the lower frequency limit for OSCs
  which depend on the power spectrum.
\item \textbf{OSCs:} \texttt{abspowerabove}, \texttt{abspowerbelow}, 
  \texttt{abspowerrange}, 
  \texttt{abspowerrise}, 
  \texttt{abspowerfall},
  \texttt{abspowergain},
  \texttt{fractpowerabove},
  \texttt{fractpowerbelow}, \texttt{fractpowerrange} 
\item \textbf{Optional:} No
\item \textbf{Data type:} Floating point
\item \textbf{Legal values:} $(-\infty, \infty)$
\item \textbf{Default value:} N/A
\end{itemize}

{\large\texttt{\textbf{freqhi}}}
\begin{itemize}
\item \textbf{Definition:} Specifies the upper frequency limit for OSCs
  which depend on the power spectrum.
\item \textbf{OSCs:} \texttt{abspowerabove}, \texttt{abspowerbelow}, 
  \texttt{abspowerrange}, 
  \texttt{abspowerrise},
  \texttt{abspowerfall},
  \texttt{abspowergain},
  \texttt{fractpowerabove}, 
  \texttt{fractpowerbelow}, \texttt{fractpowerrange} 
\item \textbf{Optional:} No
\item \textbf{Data type:} Floating point
\item \textbf{Legal values:} $(-\infty, \infty)$
\item \textbf{Default value:} N/A
\end{itemize}

{\large\texttt{\textbf{mask}}}
\begin{itemize}
\item \textbf{Definition:} The bit mask with which to operate on the
  data.
\item \textbf{OSCs:} \texttt{bitand}, \texttt{bitnand}, \texttt{bitor}, \texttt{
    bitnor}
\item \textbf{Optional:} No
\item \textbf{Data type:} Integer. (May be specified in octal or
  hexadecimal format: octal has a leading ``0'' (zero) and hexadecimal
  has a leading ``0x'' (zero x).)
\end{itemize}


\begin{center}
  \textbf{Atomic OSCs}
\end{center}

These OSCs act on the time-series data in channels, and hence require
a channel name in their definition, \textit{e.g.}:

\begin{verbatim}
    X_arm_locked  meanabove "H2:LSC-AS_I" threshold=17.3
\end{verbatim}

\textit{NOTE:} The power in the \texttt{power*} OSCs is really
band-limited RMS.


{\large\texttt{\textbf{valueabove}}}

\begin{itemize}
\item \textbf{True when:} \texttt{fraction} of data samples are greater than
  \texttt{threshold}
\item \textbf{Parameters:} \texttt{fraction}, \texttt{threshold}
\end{itemize}


{\large\texttt{\textbf{valuebelow}}}

\begin{itemize}
\item \textbf{True when:} \texttt{fraction} of data samples are less than
  \texttt{threshold}
\item \textbf{Parameters:} \texttt{fraction}, \texttt{threshold}
\end{itemize}


{\large\texttt{\textbf{valuerange}}}

\begin{itemize}
\item \textbf{True when:} \texttt{fraction} of data samples in the interval 
  $(\mathrm{lo}, \mathrm{hi})$
  and < \texttt{hi}
\item \textbf{Parameters:} \texttt{fraction}, \texttt{lo}, \texttt{hi}
\end{itemize}

{\large\texttt{\textbf{meanabove}}}

\begin{itemize}
\item \textbf{True when:} mean of data is greater than
  \texttt{threshold}
\item \textbf{Parameters:} \texttt{threshold}
\end{itemize}


{\large\texttt{\textbf{meanbelow}}}

\begin{itemize}
\item \textbf{True when:} mean of data is less than
  \texttt{threshold}
\item \textbf{Parameters:} \texttt{threshold}
\end{itemize}


{\large\texttt{\textbf{meanrange}}}

\begin{itemize}
\item \textbf{True when:} mean of data is in the interval 
  $(\mathrm{lo}, \mathrm{hi})$ 
\item \textbf{Parameters:} \texttt{lo}, \texttt{hi}
\end{itemize}

{\large\texttt{\textbf{meanrise}}}

\begin{itemize}
\item \textbf{True when:} mean of data has increased by an amount >
  \texttt{threshold} over \texttt{nstrides} strides
\item \textbf{Parameters:} \texttt{threshold}, \texttt{nstrides}
\end{itemize}

{\large\texttt{\textbf{meanfall}}}

\begin{itemize}
\item \textbf{True when:} mean of data has decreased by an amount >
  \texttt{threshold} over \texttt{nstrides} strides
\item \textbf{Parameters:} \texttt{threshold}, \texttt{nstrides}
\end{itemize}

{\large\texttt{\textbf{rmsabove}}}

\begin{itemize}
\item \textbf{True when:} RMS (full bandwidth) of data is greater than
  \texttt{threshold}
\item \textbf{Parameters:} \texttt{threshold}
\end{itemize}


{\large\texttt{\textbf{rmsbelow}}}

\begin{itemize}
\item \textbf{True when:} RMS (full bandwidth) of data is less than
  \texttt{threshold}
\item \textbf{Parameters:} \texttt{threshold}
\end{itemize}


{\large\texttt{\textbf{rmsrange}}}

\begin{itemize}
\item \textbf{True when:} RMS (full bandwidth) of data is in the interval
  $(\mathrm{lo}, \mathrm{hi})$
\item \textbf{Parameters:} \texttt{lo}, \texttt{hi}
\end{itemize}


{\large\texttt{\textbf{bitand}}}

\begin{itemize}
\item \textbf{True when:} bitwise AND of \texttt{fraction} of data
  with  \texttt{mask} is equal to \texttt{mask}
\item \textbf{Parameters:} \texttt{mask}, \texttt{fraction}
\item \textbf{Note:} \texttt{mask} may be specied in octal (leading
  \texttt{0}) or hexadecimal (leading \texttt{0x}), \textit{e.g.} 0123 is
  decimal 83, 0xff is decimal 255
\end{itemize}

{\large\texttt{\textbf{bitnand}}}

\begin{itemize}
\item \textbf{True when:} bitwise AND of \texttt{fraction} of data with \texttt{
    mask} not equal to \texttt{mask}
\item \textbf{Parameters:} \texttt{mask}, \texttt{fraction}
\item \textbf{Note:} \texttt{mask} may be specied in octal (leading
  \texttt{0})  or hexadecimal (leading \texttt{0x}),
  \textit{e.g.} 0123 is decimal 83, 0xff is decimal 255
\end{itemize}

{\large\texttt{\textbf{bitor}}}

\begin{itemize}
\item \textbf{True when:} bitwise AND of \texttt{fraction} of data with \texttt{
    mask} is not equal to 0
\item \textbf{Parameters:} \texttt{mask}, \texttt{fraction}
\item \textbf{Note:} \texttt{mask} may be specied in octal (leading
  \texttt{0})  or hexadecimal (leading \texttt{0x}),  \textit{e.g.} 0123
  is decimal 83, 0xff is decimal 255   
\end{itemize}


{\large\texttt{\textbf{bitnor}}}

\begin{itemize}
\item \textbf{True when:} bitwise AND of \texttt{fraction} of data with \texttt{
    mask} is equal to 0
\item \textbf{Parameters:} \texttt{mask}, \texttt{fraction}
\item \textbf{Note:} \texttt{mask} may be specied in octal (leading
  \texttt{0}) or hexadecimal (leading \texttt{0x}),
  \textit{e.g.} 0123 is decimal 83, 0xff is decimal 255  
\end{itemize}


{\large\texttt{\textbf{abspowerabove}}}

\begin{itemize}
\item \textbf{True when:}  power in frequency range \texttt{freqlo}
  and \texttt{freqhi} is above \texttt{threshold}

\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi}, \texttt{threshold}
\item \textbf{Note:} \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
  (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range.  This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools. This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}


{\large\texttt{\textbf{abspowerbelow}}}

\begin{itemize}
\item \textbf{True when:} power in frequency range \texttt{freqlo}
  and \texttt{freqhi} is below \texttt{threshold}
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi},
  \texttt{threshold}
\item \textbf{Note:} \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
  (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range.  
  This number is independent of stride (length of time series used to
  compute PSD). 
\end{itemize}


{\large\texttt{\textbf{abspowerrange}}}

\begin{itemize}
\item \textbf{True when:} power in frequency range \texttt{freqlo}
  and \texttt{freqhi} is between \texttt{lo} and \texttt{hi}
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi},
  \texttt{lo}, \texttt{hi}
\item \textbf{Note:} \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
 (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range.  This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools. This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}

{\large\texttt{\textbf{abspowerrise}}}
\begin{itemize}
\item \textbf{True when:} power in frequency range
  $[\mathtt{freqlo}, \mathtt{freqhi}]$ has risen by an amount
  $>$ \texttt{threshold} over \texttt{nstrides} strides
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi},
  \texttt{threshold}, \texttt{nstrides}
\item \textbf{Note:} \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
  (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range.  This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools.  This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}

{\large\texttt{\textbf{abspowerfall}}}
\begin{itemize}
\item \textbf{True when:} power in frequency range
  $[\mathtt{freqlo}, \mathtt{freqhi}]$ has fallen by an amount
  $>$ \texttt{threshold} over \texttt{nstrides} strides
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi},
  \texttt{threshold}, \texttt{nstrides}  
\item \textbf{Note:} \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
  (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range.  This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools. This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}

{\large\texttt{\textbf{abspowergain}}}
\begin{itemize}
\item \textbf{True when:} Depends on value of \texttt{threshold}.  If
  \texttt{threshold} > 1, True when power in frequency range
  $[\mathtt{freqlo}, \mathtt{freqhi}]$ is changing by a factor $>$
  \texttt{threshold} over \texttt{nstrides} strides.  If
  \texttt{threshold} < 1, True when power in frequency range is
  changing by a factor $<$ \texttt{threshold} over \texttt{nstrides}
  strides. 
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi},
  \texttt{threshold}, \texttt{nstrides}
\item \textbf{Note:} \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
  (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range.  This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools. This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}

{\large\texttt{\textbf{fractpowerabove}}}

\begin{itemize}
\item \textbf{True when:} fractional power in frequency range
  $[\mathtt{freqlo}, \mathtt{freqhi}]$ is above \texttt{threshold}
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi}, \texttt{threshold}
\item \textbf{Note:} \textit{Fractional power} is defined to be the
  ratio of power in the frequency range requested to the full-band
  power.  \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
  (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range. This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools. This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}


{\large\texttt{\textbf{fractpowerbelow}}}

\begin{itemize}
\item \textbf{True when:} fractional power in frequency range
  $[\mathtt{freqlo}, \mathtt{freqhi}]$ is below \texttt{threshold}
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi}, \texttt{threshold}
\item \textbf{Note:} \textit{Fractional power} is defined to be the
  ratio of power in the frequency range requested to the full-band
  power.  \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
  (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range. This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools. This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}


{\large\texttt{\textbf{fractpowerrange}}}

\begin{itemize}
\item \textbf{True when:} fractional power in frequency range
  $[\mathtt{freqlo}, \mathtt{freqhi}]$ is between \texttt{lo} and \texttt{hi}
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi},
  \texttt{lo}, \texttt{hi} 
\item \textbf{Note:} \textit{Fractional power} is defined to be the
  ratio of power in the frequency range requested to the full-band
  power.  \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
  (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range. This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools.  This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}

{\large\texttt{\textbf{fractpowerrise}}}

\begin{itemize}
\item \textbf{True when:} fractional power in frequency range
  $[\mathtt{freqlo}, \mathtt{freqhi}]$ has risen by an amount $>$
  \texttt{threshold} over \texttt{nstrides} strides
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi},
  \texttt{threshold}, \texttt{nstrides}
\item \textbf{Note:} \textit{Fractional power} is defined to be the
  ratio of power in the frequency range requested to the full-band
  power.  \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
  (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range.  This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools. This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}

{\large\texttt{\textbf{fractpowerfall}}}

\begin{itemize}
\item \textbf{True when:} fractional power in frequency range
  $[\mathtt{freqlo}, \mathtt{freqhi}]$ has fallen by an amount $>$
  \texttt{threshold} over \texttt{nstrides} strides
\item \textbf{Parameters:} \texttt{freqlo}, \texttt{freqhi},
  \texttt{threshold}, \texttt{nstrides}
\item \textbf{Note:} \textit{Fractional power} is defined to be the
  ratio of power in the frequency range requested to the full-band
  power.  \textit{Power} in this and all the \texttt{power}
  OSCs is defined to be the mean square value of the PSD
 (computed by calling \texttt{FSpectrum::getSum()} for the
  frequency range specified) multiplied by the frequency range.  This
  power is the square of the RMS that is plotted in the DTT Fourier
  Tools. This number is independent of stride (length of time series
  used to compute PSD). 
\end{itemize}

\begin{center}
  \textbf{Meta-OSCs}
\end{center}

{\large\texttt{\textbf{boolean}}}

\begin{itemize}
\item \textbf{True when:} Boolean expression of OSCs evaluates to True
\item \textbf{Parameters:} none
\item \textbf{Example:}\texttt{Both\_arms\_locked  boolean "X\_arm\_locked
    \& Y\_arm\_locked"}
\item \textbf{Notes:} Boolean expressions use previously defined OSC
  names as Boolean variables.  Operators supported are: \texttt{!}
  (NOT), \texttt{\&} (AND), \texttt{|} (OR).  Precedence rules follow
  those of standard logic. Parentheses may be used to make precedence
  explicit. (Ignore backslashes above in the HTML version of this
  manual.)
\end{itemize}

{\large\texttt{\textbf{transitup}}}

\begin{itemize}
\item \textbf{True when:} given OSC changes from False to True
\item \textbf{Parameters:} \texttt{hold}, \texttt{dead}
\item \textbf{Example:} \texttt{X\_arm\_lock\_acquired  transitup
    "X\_arm\_locked" hold=0}
\item \textbf{Notes:} This meta-OSC is held True for \texttt{hold} + 1 strides
\end{itemize}

{\large\texttt{\textbf{transitdown}}}

\begin{itemize}
\item \textbf{True when:} given OSC changes from True to False
\item \textbf{Parameters:} \texttt{hold}, \texttt{dead}
\item \textbf{Example:} \texttt{X\_arm\_lock\_lost  transitdown
    "X\_arm\_locked" hold=0}
\item \textbf{Notes:} This meta-OSC is held True for \texttt{hold} + 1 strides
\end{itemize}

The bitwise conditions merit further explanation. The \texttt{bitand} and
\texttt{bitnand} conditions refer to whether \textit{every} bit in the
bitmask has corresponds to a bit in the data channel value. The \texttt{
bitor} and \texttt{bitnor} conditions refer to whether \textit{at least one}
of the bits in the bitmask has a corresponding bit in the data channel
value. Hence for bit masks with only one bit turned on, the \texttt{and}
and \texttt{or} conditions are identical.

\texttt{transitup} and \texttt{transitdown} are conditions on other
OSCs. \texttt{transitup} becomes True and is held True for $N+1$ strides
whenever the named OSC changes state from False to True, where $N$ is
the hold duration parameter. \texttt{transitdown} works in a similar way,
except that it becomes True when the named OSC goes from True to
False.  The \texttt{dead} parameter prevents \texttt{transitup} and \texttt{
transitdown} conditions from becoming True again for an interval of
\texttt{dead} strides after the end of the \texttt{hold} period.

One cannot yet define OSCs in code, only via a configuration file.


\begin{center}
  \textbf{{\large Other Features}}
\end{center}

The \texttt{OperStateCond} objects have some features worth noting:

\begin{itemize}
\item All the \textit{directive}s that are legal in a configuration file have
   a corresponding method.
\item All atomic OSCs have a \texttt{watchedQuantity()} method which returns
   the most recent \texttt{double} value of the quantity being monitored,
   \textit{e.g} the mean value for a \textt{mean*} condition, the
   power for a \texttt{power*} condition.
\end{itemize}


\begin{center}
  \textbf{{\large FIXME: Old Stuff}}
\end{center}



The following spectral power conditions (meant for detecting
transients, including servo instabilities) require specifying a
minimum and maximum frequency for the range of computed power. The
power is normalized so that its sum from zero to the Nyquist frequency
equals the mean square value of the corresponding time
series. Conditions on fractional power refer to the fraction of total
power in the specified frequency range.  Only parameters additional to
the min/max frequencies are listed in the following table.

The ``mag'' and ``demag'' conditions refer to fractional changes in
  power over a given frequency range, independent of the absolute power
level and of power in other frequency ranges. Normally, the scale
factor used for ``mag'' should be greater than one and that used for
``demag'' less than one. The ``any'' and ``ave'' modifiers have
similar meanings as for power rises and falls, except that for ``any''
the instantaneous rate threshold is taken as (scale factor)**(1 /
number of time intervals). The number of time intervals N must be a
positive integer. For ``ave'' conditions, at least N+1 time
consecutive intervals must be examined before the condition can be
satisfied.

Provision for these various ways of defining power changes has been
made for flexibility. Very stable channels with nearly constant total
power may be well suited to conditions based on absolute power.
Channels with time-dependent total power but stable spectral shape may
be well suited to conditions on fractional power. Channels with
variations in total power and spectral shape may be best suited to
conditions on power magnification scale factors.





\begin{center}
\textbf{{\large Checking conditions}}
\end{center}

If an OSC has been defined during initialization, then one can check
whether it is satisfied during a given time interval with a call to
the \texttt{satisfied()} method. \textit{e.g.}:

\begin{verbatim}
    if (osclist.satisfied("oscname"))
        // send a trigger
\end{verbatim}

The \texttt{OperStateCondList} class inherits from a \texttt{hash\_map<const
  string, osc::OperStateCond*, osc::hash<const string> >}, so all the
methods that a \texttt{hash\_map} has are available.  (See SGI's
documentation for the Standard Template Library at
\texttt{http://www.sgi.com/tech/stl/}.) So, for instance, one can loop
  through all defined conditions:
  
\begin{verbatim}
    // As each time interval of data is read:
    OperStateCondList::iterator iter = osclist.begin();
    for (; iter != osclist.end(); ++iter) {
        if ((*iter).second->satisfied() == true)
            cout << "'" << (*iter).first << "'\tsatisfied" << endl;
        else
            cout << "'" << (*iter).first << "'\t\tNOT satisfied" 
                 << endl;
    }
\end{verbatim}
  
\begin{center}
  \textbf{{\large Other Facilities}}
\end{center}

In addition to the \texttt{OperStateCondList} class, a helper class
called \texttt{TSWindow} is also available.  \texttt{TSWindow}
represents a time series with at most $N$ elements.  It is typically
used as a ``window'' on a time series that is to be presented to the
DMT Viewer.  For a relatively simple example, see the source code for
the \texttt{LockLoss} monitor:
\texttt{~dmt/cvs/dmt/src/monitors/LockLoss}. To add a datapoint to a
\texttt{TSWindow} object, use the \texttt{append()} method.

\newpage

\begin{center}\textbf{{\large Hacker's Guide}}\end{center}

\begin{center}
  \textbf{Big picture from the end-user standpoint}
\end{center}

We want to have various Operational State Conditions, encapsulated by
the class \texttt{OperStateCond} (and abbreviated OSC for the rest of
this document), which specify conditions which may or may not be
satisfied by the data stream in \textit{one (1)} channel.

Then, for the user, there is a \texttt{hash\_map} of standard OSCs,
\textit{e.g.} \texttt{valueabove} and \texttt{meanbelow}, indexed by
the user-specified names defined in a configuration file. This is so
that the user can refer to specific OSCs.

Each OSC has none or some parameters associated with it, \textit{e.g.}
\texttt{valueabove} has two parameters, one of which is its
\texttt{threshold}: if the data in the channel ever goes above that
threshold value, the OSC is said to be ``satisfied'' (the
\texttt{satisfied()} member function).

Each parameter has a name, \textit{e.g.} \texttt{threshold}, and a
datatype, \textit{e.g.} \texttt{double}.

This code will read a configuration file specifying the various OSCs
and the corresponding parameter values.  Each OSC must be given a
unique name, a string of alphanumeric characters beginning with an
alphabetic character.

The reason for naming each OSC is so that each OSC maybe referred to
later in a configuration file and used in meta-OSCs.  Since each
OSC.\texttt{satisfied()} returns a \texttt{boolean}, one may now define a
Boolean OSC to say something like ``True if CHAN1 has valueabove 2.3
AND CHAN2 has meanbelow 4.2''.

Rather than having to manually type in the names of each OSC into the
monitor code, we define the \texttt{OperStateCondList} class, which,
despite its name, is not a linked-list but a \texttt{hash\_map}, \textit{
i.e.} a dictionary that associates a pointer to an \texttt{OperStateCond}
object with a string (the user-defined name of the OSC, specified in
the configuration file).  Making \texttt{OperStateCondList} a \texttt{
hash\_map} means that we may iterate over all defined OSCs:

\begin{verbatim}

  OperStateCondList mOSClist;
  OperStateCondList::iterator iter = mOSClist.begin();
  for (; iter != mOSClist.end(); ++iter)
         if ((*iter).second->satisfied())
             // generate and send a trigger to the MetaDB

\end{verbatim}

Notice that this still does not obviate the need for \texttt{
OperStateCondList::satisfied(const char *oscname)} since the user
might want to test for a particular OSC, and the method was defined in
a previous incarnation of this code.  Another way of accessing
particular OSCs is to use the fact that it's a \texttt{hash\_map}
(which is actually what \texttt{OperStateCondList::satisfied()} does):

\begin{verbatim}
  OperStateCondList mOSClist;
  mOSClist.readConfig("configfile.conf");
  
  if (mOSClist["my_condition"]->satisfied())
     cout << "my_condition is satisfied" << endl;
  else
     cout << "my_condition is NOT satisfied" << endl;
\end{verbatim}


\begin{center}
  \textbf{Big picture from the programmer's standpoint}
\end{center}

This is more info for one who wishes to modify the OperStateCondList
code.  If you read the above, you will see a very striking resemblance
between OSCs and the basic datatypes of a programming language.  In
a programming language, we may define variables of different
datatypes; in the OperStateCondList system, we may define OSCs of
different OSC types (like \texttt{valueabove}).

And then, the Boolean OSCs are just a Boolean expression of previously
defined OSCs.  So, this really becomes a small language, and hence the
scanning, tokenizing and parsing objects associated with the Boolean
OSC.  (See \texttt{osc/boolean/*.hh}.)

\texttt{boolean}, \texttt{transitup}, and \texttt{transitdown} OSCs are really
meta-OSCs, \textit{i.e.} they define conditions on other OSCs.  So, to
know whether the OSCs they refer to are satisfied, they need to get a
pointer to the \texttt{OperStateCondList} that contains them.

To make the evaluation of the Boolean expression a little more
efficient, we convert the infix form of the Boolean expression, \textit{
e.g.} \texttt{Locklost \&\ PSLglitch}, to Reverse Polish Notation
(RPN), and in the process verify the correctness of the expression.
RPN is quicker to evaluate since it simplifies putting the running
value in a stack.

To make evaluations for the other conditions a little more efficient,
we store a short history of two \texttt{satisfied()} values: the value
for the current stride, and the value for the previous stride.  Some
OSCs depend on more than just the previous stride, so they have their
own private data members defined for storing this data. 

Now, for a larger overview.  And let's do this in a top-down way, with
a very large grain.

We've already seen that \texttt{OperStateCondList} is a \texttt{
hash\_map<const string, OperStateCond*>}.

Any new types of OSCs that are to be written will have to inherit from
\texttt{OperStateCond}: it is a base class that provides data and
functions that all OSCs need.

\texttt{OperStateCondList} also contains (`has a' relationship)
information about the various basic OSC types.  This information is
stored in \texttt{osc::TypeInfoMap mTypeInfoMap}.  (See \texttt{
OSCListMisc.hh}.)

\texttt{osc::typeInfoMap} is a \texttt{hash\_map<const string, TypeInfo>},
\textit{i.e.}  it's a dictionary that relates a \texttt{TypeInfo} object to
a \texttt{const string} (which is the name of that type, \textit{e.g.} \texttt{
valueabove}).

The \texttt{TypeInfo} class contains information about the OSC type.
It does this by containing a sorted map of parameter types:
\texttt{typedef map<const string, tseriesType\_t> ParamTypeMap}.
\texttt{tseriesType\_t} is defined in \texttt{namespace osc} and is
an enumeration of the various ``atomic'' data types that may be
contained in a \texttt{TSeries}.  The indexing key for
\texttt{ParamTypeMap} is the name of the parameter, \textit{e.g.}
\texttt{mParamTypeMap["threshold"]} would be a \texttt{DOUBLE}.

In summary, let ``*'' mean ``has a'', and ``>'' mean ``is a''.

\begin{verbatim}
OperStateCondList
 > hash_map of OperStateCond*, indexed by user-defined names of OSCs
 * TypeInfoMap, containing information about the basic types

OperStateCond
 > a base class from which actual OSCs will inherit
 * a bunch of stuff.  See OperStateCond.hh and misc.hh

TypeInfoMap 
 > hash_map of TypeInfo, indexed by names of types
 * ParamInfoMap, containing information about the parameters
                 associated with this type of OSC

ParamInfoMap 
 > hash_map of ParamInfo, indexed by names of parameters

\end{verbatim}

The \texttt{ParamInfoMap} and \texttt{TypeInfoMap} are initialized in 
\texttt{OperStateCondList}'s constructor.  Since this data should be common
to all instances of \texttt{OperStateCondList}, one would like this to be
a static data member, but unfortunately it is almost impossible to
initialize a complex data structure such as this at compile time.

Now, how do the OSCs store the values of their parameters?  Since
these parameters may be of different types, and they may be read from
an Epics channel, we can't just store them as simple values.  We use a
template class (just because I don't know how to do something like the
\texttt{Param} class in DMT: see \texttt{osc/Param.hh}).  For now, we
only have \texttt{int} and \texttt{double} parameters, so each
\texttt{OperStateCond} object has two \texttt{hash\_map}s: one for
integer parameters (\texttt{mIntParams}) and one for doubles
(\texttt{mDblParams}).  These two \texttt{hash\_map}s are indexed by
the names of the parameters. The value of a parameter may be retrieved
by, \textit{e.g.}:

\begin{verbatim}
    mDblParams["threshold"].value()
\end{verbatim}

\begin{center}
  \textbf{How to Add New OSC Types}
\end{center}

Look in the \texttt{osc} subdirectory.  You'll create a new class that
inherits from \texttt{OperStateCond}.  Look at the others for
examples.  You'll have to initialize the object's
\texttt{ParamInfoMap} in the \texttt{OperStateCondList} constructor,
and then enter that into \texttt{OperStateCondList}'s
\texttt{TypeInfoMap}.  You will need to add a section that actually
adds a new OSC to the \texttt{hash\_map} (around line no. 612 and
onwards).  You may need to write sections to parse the config file
line for your new OSC, though it's quite unlikely unless you have a
new type of parameter (\textit{e.g} a complex number) that behaves
very differently from the types that already exist.
\texttt{OperStateCondList::parseAtomicParams()} is the method that
parses config file lines for OSCs.  The \texttt{transit*} conditions
have config line parsing written inline in the constructor of
\texttt{OperStateCondList}.  Only the \texttt{boolean} OSC type has
its own parser since it's a bit of a trick to parse a Boolean
expression.

*/

//@}
