\NeedsTeXFormat{LaTeX2e}

\DeclareOption*{\PassOptionsToClass{\CurrentOption}{report}}
\ProcessOptions

\LoadClass{report}

\RequirePackage{a4}
\RequirePackage[english]{babel}
\RequirePackage[T1]{fontenc}
\RequirePackage[latin1]{inputenc}
\RequirePackage[dvips]{graphicx}
\RequirePackage{lastpage}
\RequirePackage{longtable}
\RequirePackage{epsfig}
\RequirePackage{url}
\RequirePackage{makeidx}

% ------------------------------------------------------------------
%  FORMAT SETTINGS
% ------------------------------------------------------------------

\setlength{\parindent}{0pt}
\setlength{\parskip}{.7ex plus .1ex minus .1ex}

\pagestyle{empty}

\def\vckey$#1${\texttt{\small #1}}

\setlength{\headheight}{15mm}
\setlength{\topmargin}{0pt}
\addtolength{\voffset}{-20mm}
\addtolength{\textheight}{10mm}
\addtolength{\textwidth}{20mm}
\addtolength{\oddsidemargin}{-12mm}

\setcounter{secnumdepth}{3}
\setcounter{tocdepth}{2}

\renewcommand{\topfraction}{.99}
\renewcommand{\bottomfraction}{.99}
\renewcommand{\textfraction}{.02}

% ------------------------------------------------------------------
%  DOCUMENT PARAMETRES
% ------------------------------------------------------------------
%% Required information about the document.

\global\let\project\empty

\global\let\title\empty
\global\let\ver\empty
\global\let\lastday\empty
\global\let\firstday\empty
\global\let\docnum\empty
\global\let\state\empty
\global\let\doctype\empty

% ------------------------------------------------------------------
%  COMMANDS
% ------------------------------------------------------------------

\newcommand{\theproject}[0]{TTA-based Co-design Environment}
\newcommand{\theschool}[0]{Tampere University of Technology}
\newcommand{\theinstitute}[0]{Department of Pervasive Computing}
\newcommand{\thegroup}[0]{Customized Parallel Computing group}

\newcommand{\note}[1]{\marginpar[\scriptsize\sf #1]{\scriptsize\sf #1}}
\newcommand{\texttilde}{{\large\raisebox{-0.75ex}{\~{}}}}
\newcommand{\notUpToDate}[0]{%
\begin{quote}\small\centering
  \begin{tabular}{|p{0.74\textwidth}|}
    \hline
    \rule{0pt}{2.5ex}%
  \textbf{Warning:} This section is not up to date. See the API reference
  documentation for complete, up to date information about the components
  described here.%
    \rule[-1.5ex]{0pt}{2.5ex}\\
    \hline
  \end{tabular}
\end{quote}
}


\global\let\thedoctype\empty
\renewcommand{\doctype}[1]{\renewcommand{\thedoctype}{#1}}
\global\let\thetitle\empty
\renewcommand{\title}[1]{\renewcommand{\thetitle}{#1\\[1ex]\thedoctype}}
\global\let\theauthor\empty
\renewcommand{\author}[1]{\renewcommand{\theauthor}{#1}}
\global\let\thever\empty
\renewcommand{\ver}[1]{\renewcommand{\thever}{#1}}
\global\let\thefirstday\empty
\renewcommand{\firstday}[1]{\renewcommand{\thefirstday}{#1}}
\global\let\thelastday\empty
\renewcommand{\lastday}[1]{\renewcommand{\thelastday}{#1}}
\global\let\thedocnum\empty
\renewcommand{\docnum}[1]{\renewcommand{\thedocnum}{#1}}
\global\let\thestate\empty
\renewcommand{\state}[1]{\renewcommand{\thestate}{#1}}

\def\today{\number\day.\number\month.\number\year}

% Make the title page
\renewcommand{\maketitle}{\begin{titlepage}%
\vspace*{4cm}
\begin{center}
%  {\LARGE \theproject}\\[2ex]
  {\LARGE \thegroup}\\[2ex]
  {\LARGE \theinstitute}\\[2ex]
  {\LARGE \theschool}\\[1ex]
  \vrule width 0.7\textwidth height 1pt depth 0pt \\[8ex]
  {\Huge \thetitle}\\[2ex]
  {\LARGE \theauthor}
\end{center}
\vspace*{\stretch{1}}

\begin{minipage}[b]{0.5\textwidth}
Version: \thever\\
Created: \thefirstday\\
Last Modified: \thelastday\\
Document: \thedoccode-\thedocnum\\
State: \thestate
\end{minipage}
\end{titlepage}}

% Add here new command definitions that can be used in all types
% of documents.
\newcommand{\shellcmd}[1]{\texttt{\hbox to 0.5cm{}#1}}
\newcommand{\file}[1]{'#1'}

% ------------------------------------------------------------------
%  ENVIRONMENTS
% ------------------------------------------------------------------

\newenvironment{HistoryTable}[0]{%
\begin{center}
\begin{longtable}{p{0.08\textwidth}p{0.10\textwidth}
                  p{0.16\textwidth}p{0.55\textwidth}}
Version & Date      & Author      &     Comment\\
\hline
\\
}{%
\\
\hline
\end{longtable}
\end{center}
}

% Add here new environment definitions that can be used in all types
% of documents.

% ------------------------------------------------------------------
%  HEADERS AND FOOTERS
% ------------------------------------------------------------------

\def\ps@reqpage{%
    \def\@oddfoot{\textrm{version \thever}\hfill\textrm{\thelastday}\hfill\thepage/\pageref{LastPage}}
    \def\@evenfoot{\thepage/\pageref{LastPage}\hfill\textrm{\thelastday}\hfill\textrm{version \thever}}
    \def\@evenhead{\textrm{\thetitle}\\}%
    \def\@oddhead{\textsc{\theproject}\\}%
    \let\@mkboth\@gobbletwo
    \let\chaptermark\@gobble
    \let\sectionmark\@gobble
    }
\let\ps@plain\ps@reqpage

\ps@reqpage
