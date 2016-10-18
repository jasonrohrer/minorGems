; Modification History
;;
;; 2001-March-10   Jason Rohrer
;; Created.
;; Fixed so that c++ mode is used for .h files.
;; Fixed [C-backspace] mapping.
;;
;; 2001-May-21   Jason Rohrer
;; Changed so that Java inclass items are not doubly indented.
;;
;; 2002-January-21   Jason Rohrer
;; Gave in and set to use spaces instead of tabs.
;; This is primarily motivated by the fact that tabs look terrible
;; in CVS-web.
;;
;; 2005-January-16   Jason Rohrer
;; Added php mode.
;;
;; 2010-January-7   Jason Rohrer
;; Added count-words function (nice!)
;;


(custom-set-variables
  ;; custom-set-variables was added by Custom -- don't edit or cut/paste it!
  ;; Your init file should contain only one such instance.
 '(case-fold-search t)
 '(current-language-environment "ASCII")
 '(global-font-lock-mode t nil (font-lock))
 '(tab-stop-list (quote (4 8 12 16 20 24 28 32 36 40 44 48 52 56 60)))
 '(transient-mark-mode t))
(custom-set-faces
  ;; custom-set-faces was added by Custom -- don't edit or cut/paste it!
  ;; Your init file should contain only one such instance.
 )


;(setq-default scroll-step 1)               ; turn off jumpy scroll
(setq-default visible-bell t)              ; no beeps, flash on errors

(display-time)                             ; display the time on modeline
(column-number-mode t)                     ; display the column number on modeline
(setq-default kill-whole-line t)           ; ctrl-k kills whole line if at col 0
(setq-default fill-column 80)              ; wrap at col 80
(setq-default tab-width 4)                 ; show tabs as 4 cols

(setq font-lock-maximum-decoration t)      ; use colours in font lock mode
(setq font-lock-maximum-size nil)          ; turn off limit on font lock mode

; turn on font-lock everywhere (if possible)
(global-font-lock-mode 1)

; make sure C-backspace kills a word
( global-set-key `[C-backspace]   `backward-kill-word )

; background color, light yellow
( add-to-list  'default-frame-alist '( background-color . "#FFFFDC" ) )

; auto fill mode
( add-hook 'c-mode-common-hook 'turn-on-auto-fill )

; these don't work in emacs 23
; fix comment syntax highlighting problems
;( setq font-lock-support-mode 'lazy-lock-mode )
;( setq lazy-lock-defer-time 0 )

; delete highlighted text as we type
( delete-selection-mode t )

; make sure c++ mode is used for .h files
( setq auto-mode-alist ( 
	append '(
		("\\.h$" . c++-mode)
		)
	auto-mode-alist
	)
)


; off for now, since it screws up indenting
; load the line numbers package
;( load "setnu" );
; turn it on
;( add-hook 'c-mode-common-hook 'turn-on-setnu-mode )


( copy-face 'italic  'font-lock-comment-face )
( set-face-foreground 'font-lock-comment-face "#00AA00" )
( setq font-lock-comment-face 'font-lock-comment-face )

( copy-face 'default 'font-lock-string-face )
( set-face-foreground 'font-lock-string-face "#FF00FF" )
( setq font-lock-string-face 'font-lock-string-face )

( copy-face 'bold 'font-lock-type-face )
( set-face-foreground 'font-lock-type-face "#0000FF" )
( setq font-lock-type-face 'font-lock-type-face )

( copy-face 'bold 'font-lock-keyword-face )
( set-face-foreground 'font-lock-keyword-face "#C88000" )
( setq font-lock-keyword-face 'font-lock-keyword-face )


( copy-face 'default 'font-lock-function-name-face )
( set-face-foreground 'font-lock-function-name-face "#C80000" )
( setq font-lock-function-name-face 'font-lock-function-name-face )

( copy-face 'default 'font-lock-variable-name-face )
( set-face-foreground 'font-lock-variable-name-face "#0000FF" )
( setq font-lock-variable-name-face 'font-lock-variable-name-face )

( copy-face 'bold 'font-lock-constant-face )
( set-face-foreground 'font-lock-constant-face "#C800FF" )
( setq font-lock-constant-face 'font-lock-constant-face )

( copy-face 'bold 'font-lock-warning-face )
( set-face-foreground 'font-lock-warning-face "#FF0000" )
( setq font-lock-warning-face 'font-lock-warning-face )

( copy-face 'default 'font-lock-reference-face )
( set-face-foreground 'font-lock-reference-face "#00FFFF" )
( setq font-lock-reference-face 'font-lock-reference-face )




(defconst my-c-style
  '((c-tab-always-indent           . t)
    (c-comment-only-line-offset    . 4)
    (c-hanging-braces-alist        . ((substatement-open after)
									  (defun-open after)
									  (block-open after)
									  (class-open after)
									  (inline-open after)
									  (brace-entry-open after)
                                      (brace-list-open)))
    (c-hanging-colons-alist        . ((member-init-intro before)
                                      (inher-intro)
                                      (case-label after)
                                      (label after)
                                      (access-label after)))
    (c-cleanup-list                . (scope-operator
                                      empty-defun-braces
                                      defun-close-semi))
    (c-offsets-alist               . ((arglist-close     . c-lineup-arglist)
                                      (substatement-open . 0)
                                      (case-label        . 4)
                                      (block-open        . 0)
									  (comment-intro     . 0)
                                      (block-close       . 4)
                                      (inclass           . 8)
                                      (access-label      . -4)
									  (defun-close       . 4)
									  (inline-close      . 4)
									  (class-close       . 4)
									  (brace-list-close  . 4)
                                      (knr-argdecl-intro . -)))
    (c-echo-syntactic-information-p . t)
    )
  "My C Programming Style")

;; Customizations for all of c-mode, c++-mode, and objc-mode
(defun my-c-mode-common-hook ()
  ;; add my personal style and set it for the current buffer
  (c-add-style "PERSONAL" my-c-style t)

  ;; offset customizations not in my-c-style
  (c-set-offset 'member-init-intro '++)
  ;; other customizations
  (setq tab-width 4
        ;; this will make sure spaces are used instead of tabs
        indent-tabs-mode nil)
  ;; we like auto-newline and hungry-delete
  (c-toggle-auto-hungry-state 1)
  ;; keybindings for C, C++, and Objective-C.  We can put these in
  ;; c-mode-map because c++-mode-map and objc-mode-map inherit it
  (define-key c-mode-map "\C-m" 'newline-and-indent)
  )

;; the following only works in Emacs 19
;; Emacs 18ers can use (setq c-mode-common-hook 'my-c-mode-common-hook)
(add-hook 'c-mode-common-hook 'my-c-mode-common-hook)
(add-hook 'java-mode-hook 'my-c-mode-common-hook)




( add-hook 'perl-mode-hook 'my-perl-mode-hook )




(require 'php-mode)


; caml mode (Tuareg)
(setq auto-mode-alist (cons '("\\.ml\\w?" . tuareg-mode) auto-mode-alist))
  (autoload 'tuareg-mode "tuareg" "Major mode for editing Caml code" t)
  (autoload 'camldebug "camldebug" "Run the Caml debugger" t)

; old caml mode
;(setq auto-mode-alist
;	  (cons '("\\.ml[iylp]?$" . caml-mode) auto-mode-alist))
;(autoload 'caml-mode "caml" "Major mode for editing Caml code." t)
;(autoload 'run-caml "inf-caml" "Run an inferior Caml process." t)
;(if window-system (require 'caml-font))
(put 'downcase-region 'disabled nil)



;; actionscript (for flash development)
(autoload 'actionscript-mode "actionscript-mode" "Major mode for actionscript." t)
;; Activate actionscript-mode for any files ending in .as
(add-to-list 'auto-mode-alist '("\\.as$" . actionscript-mode))
;; Load our actionscript-mode extensions.
;;(eval-after-load "actionscript-mode" '(load "as-config"))



;; October 22, 2010

;;; Final version: while
(defun count-words-region (beginning end)
  "Print number of words in the region."
  (interactive "r")
  (message "Counting words in region ... ")

;;; 1. Set up appropriate conditions.
  (save-excursion
    (let ((count 0))
      (goto-char beginning)

;;; 2. Run the while loop.
      (while (and (< (point) end)
                  (re-search-forward "\\w+\\W*" end t))
        (setq count (1+ count)))

;;; 3. Send a message to the user.
      (cond ((zerop count)
             (message
              "The region does NOT have any words."))
            ((= 1 count)
             (message
              "The region has 1 word."))
            (t
             (message
              "The region has %d words." count))))))

;; Count the words in the entire document
(defun count-words-buffer ()
  "Count all the words in the buffer"
  (interactive)
  (count-words-region (point-min) (point-max) )
)
 


;; so that gcc smart quotes are displayed properly
(prefer-coding-system 'utf-8)



;; don't show help screen at startup
(setq inhibit-startup-message t)
(setq inhibit-splash-screen t)
