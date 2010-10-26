import itertools

def constant_factory(value):
    return itertools.repeat(value).next

def dependence_label(dep):
    """Return a string representing dep."""
    reason, kind = dep
    if kind == u'trg':
        label = "%s" % (kind, )
    elif kind != u'unknown':
        label = "%s_%s" % (reason, kind)
    else:
        label = "%s" % reason
    return label

