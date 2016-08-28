/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
*/
/******************************************************************************/


// StringId declaration
//-----------------------------------------------------------------------------

class StringId
{
  public:
    
    typedef long long Storage;
    static_assert(sizeof(Storage) == 8, "StringId requires 64-bit storage");

    static const StringId Concat(const StringId &sid, const char *str);

    StringId();
    explicit StringId(Storage data);
    explicit StringId(const char *str);

    const StringId Concat(const char *str) const;

    constexpr const Storage GetValue() const { return m_data; }

  private:
    
    Storage m_data;
};

std::ostream& operator<<(std::ostream &out, StringId sid)
{
    return out << "sid:" << sid.GetValue();
}

bool operator==(const StringId& lhs, const StringId &rhs)
{
    return lhs.GetValue() == rhs.GetValue();
}

bool operator!=(const StringId& lhs, const StringId &rhs)
{
  return lhs.GetValue() != rhs.GetValue();
}

//-----------------------------------------------------------------------------
// end: stringId declaration


// FNV hash
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
//-----------------------------------------------------------------------------

constexpr StringId::Storage StringIdHashConcat(StringId::Storage base, const char *str)
{
    return (*str) ? (StringIdHashConcat((base * 0x100000001b3) ^ *str, str + 1)) : base;
}

const constexpr StringId::Storage StringIdHash(const char *str)
{
    return StringIdHashConcat(0xcbf29ce484222325, str);
}

//-----------------------------------------------------------------------------
// end FNV hash


// StringId definition
//-----------------------------------------------------------------------------

StringId::StringId()
  : m_data(0)
{ }

StringId::StringId(Storage data)
  : m_data(data)
{ }

StringId::StringId(const char *str)
  : m_data(StringIdHash(str))
{ }

const StringId StringId::Concat(const StringId &sid, const char *str)
{
  return sid.Concat(str);
}

const StringId StringId::Concat(const char *str) const
{
  return StringId(StringIdHashConcat(m_data, str));
}

//-----------------------------------------------------------------------------
// end: StringId definition


// StringId macros & constants
//-----------------------------------------------------------------------------

#define SID(str) (StringId(str))
#define SID_VAL(str) (StringIdHash(str))
static const StringId kInvalidStringId(static_cast<StringId::Storage>(0));

//-----------------------------------------------------------------------------
// end: StringId macros & constants
