#ifndef LIB
#define LIB
/****************************/
class _map
{
private:
    /*******************************/
    void add(int id,string Name)
    {
      cmap.insert(pair<int,string>(id,Name)) ;
    }
    /*******************************/
    void list()
    {
        for (auto it = Gmap.begin(); it != Gmap.end(); ++it)
          {
              std::cout << it->first << " : " << it->second << endl;
          }
    }
    /*******************************/
    void destroid(int id)
    {
        itMap= cmap.find(id);
        Gmap.erase(itMap,itMap);
    }
public:
    std::shared_ptr <map <int,string>> cmap;

  void _map_main(char c,int i,string s)
  {
       switch (c)
      {
         case "1":
           add(i,s);
         case "2":
           list();
         case "3":
           destroid(i);
      }
  }
};
/*******************************/
void die (const char *format, ...)
{
  va_list args;
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  fprintf (stderr, "\n");
  exit (1);
}
/*******************************/
 void *xmalloc (size_t size)
{
  void *rv;
  if (size == 0)
    return NULL;
  rv = malloc (size);
  if (rv == NULL)
    die ("out-of-memory allocating %u bytes", (unsigned) size);
  return rv;
}
/*******************************/
 void chomp_trailing_whitespace (char *buf)
{
  unsigned len = strlen (buf);
  while (len > 0)
    {
      if (!isspace (buf[len-1]))
        break;
      len--;
    }
  buf[len] = 0;
}
/*******************************/
 void handle_query_response (const Foo__LookupResult *result,void *closure_data)
{
  if (result == NULL)
    cout<< "Error processing request.\n";
  else if (result->person == NULL)
    cout<<"Not found.\n";
  else
    {
      Foo__Person *person = result->person;
      unsigned i;
      cout<< person->name<< person->id;
      if (person->email)
        cout<<  person->email;
      for (i = 0; i < person->n_phone; i++)
        {
          const ProtobufCEnumValue *ev;
          ev = protobuf_c_enum_descriptor_get_value (&foo__person__phone_type__descriptor, person->phone[i]->type);
         cout<<ev ? ev->name : "???"<<person->phone[i]->number;
        }
    }

  * (protobuf_c_boolean *) closure_data = 1;
}
 /*******************************/
 void *xrealloc (void *a, size_t size)
{
  void *rv;
  if (size == 0)
    {
      free (a);
      return NULL;
    }
  if (a == NULL)
    return xmalloc (size);
  rv = realloc (a, size);
  if (rv == NULL)
    die ("out-of-memory re-allocating %u bytes", (unsigned) size);
  return rv;
}
/*************************************/
 char *xstrdup (const char *str)
{
  if (str == NULL)
    return NULL;
  return strcpy (xmalloc (strlen (str) + 1), str);
}
/**************************************/
 char *peek_next_token (char *buf)
{
  while (*buf && !isspace (*buf))
    buf++;
  while (*buf && isspace (*buf))
    buf++;
  return buf;
}
/***************************************/

int compare_persons_by_name (const void *a, const void *b)
{
  return strcmp (((const Foo__Person*)a)->name, ((const Foo__Person*)b)->name);
}
/***************************************/
 void load_database (const char *filename)
{
  FILE *fp = fopen (filename, "r");
  char buf[2048];
  unsigned n_people = 0;
  unsigned people_alloced = 32;
  unsigned line_no;
  Foo__Person *people = xmalloc (sizeof (Foo__Person) * people_alloced);
  if (fp == NULL)
    die ("error opening %s: %s", filename, strerror (errno));
  line_no = 0;
  while (fgets (buf, sizeof (buf), fp) != NULL)
    {
      line_no++;
      if (buf[0] == '#')
    continue;
      if (is_whitespace (buf))
        continue;
      chomp_trailing_whitespace (buf);
      if (isspace (buf[0]))
        {
      Foo__Person *person;
          char *start = buf + 1;
      if (n_people == 0)
        die ("error on %s, line %u: line began with a space, but no person's name preceded it",
             filename, line_no);
          person = people + (n_people - 1);
          while (*start && isspace (*start))
            start++;
          if (starts_with (start, "id "))
            person->id = atoi (peek_next_token (start));
          else if (starts_with (start, "email "))
            person->email = xstrdup (peek_next_token (start));
          else if (starts_with (start, "mobile ")
              ||   starts_with (start, "home ")
              ||   starts_with (start, "work "))
            {
              Foo__Person__PhoneNumber *pn = xmalloc (sizeof (Foo__Person__PhoneNumber));
              Foo__Person__PhoneNumber tmp = FOO__PERSON__PHONE_NUMBER__INIT;
              tmp.has_type = 1;
              tmp.type = start[0] == 'm' ? FOO__PERSON__PHONE_TYPE__MOBILE
                       : start[0] == 'h' ? FOO__PERSON__PHONE_TYPE__HOME
                       : FOO__PERSON__PHONE_TYPE__WORK;
              tmp.number = xstrdup (peek_next_token (start));
              person->phone = xrealloc (person->phone, sizeof (Foo__Person__PhoneNumber*) * (person->n_phone+1));
              *pn = tmp;
              person->phone[person->n_phone++] = pn;
            }
          else
            die ("error on %s, line %u: unrecognized field starting with %s", filename, line_no, start);
    }
      else
        {
      Foo__Person *person;
      if (n_people == people_alloced)
        {
              people_alloced *= 2;
              people = xrealloc (people, people_alloced * sizeof (Foo__Person));
        }
      person = people + n_people++;
      foo__person__init (person);
      person->name = xstrdup (buf);
    }
    }
  if (n_people == 0)
    die ("empty database: insufficiently interesting to procede");

  qsort (people, n_people, sizeof (Foo__Person), compare_persons_by_name);

  database = people;
  database_size = n_people;
  fclose (fp);
}
/*******************************/
int compare_name_to_person (const void *a, const void *b)
{
  return strcmp (a, ((const Foo__Person*)b)->name);
}
/*******************************/
void example__by_name (Foo__DirLookup_Service    *service,const Foo__Name *name,Foo__LookupResult_Closure  closure,void *closure_data)
{
  (void) service;
  if (name == NULL || name->name == NULL)
    closure (NULL, closure_data);
  else
    {
      Foo__LookupResult result = FOO__LOOKUP_RESULT__INIT;
      Foo__Person *rv = bsearch (name->name, database, database_size,sizeof (Foo__Person), compare_name_to_person);
      if (rv != NULL)
        result.person = rv;
      closure (&result, closure_data);
    }
}
/*******************************/


void err(const char* s) {
  perror(s);
  exit(EXIT_FAILURE);
}
#endif // LIB

